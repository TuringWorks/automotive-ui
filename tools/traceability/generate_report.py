#!/usr/bin/env python3
"""
Traceability Report Generator

Generates traceability matrices and reports linking:
- Safety requirements (SR-*, FSR-*, TSR-*) to implementation
- Security requirements (CR-*) to implementation
- Requirements to test cases
- Test cases to results

Usage:
    python generate_report.py [--format html|json|markdown] [--output dir]
"""

import argparse
import json
import os
import re
import sys
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Set


@dataclass
class Requirement:
    """Represents a requirement with traceability links."""
    id: str
    title: str
    category: str  # 'safety' or 'security'
    source_file: str
    source_line: int
    implementations: List[str] = field(default_factory=list)
    tests: List[str] = field(default_factory=list)
    status: str = "unverified"


@dataclass
class Implementation:
    """Represents a code implementation."""
    file: str
    line: int
    function: str
    requirement_ids: List[str] = field(default_factory=list)


@dataclass
class TestCase:
    """Represents a test case."""
    id: str
    description: str
    requirement_ids: List[str] = field(default_factory=list)
    result: Optional[str] = None  # 'pass', 'fail', None


class TraceabilityAnalyzer:
    """Analyzes codebase for traceability information."""

    # Patterns for requirement references in code/docs
    REQ_PATTERNS = {
        'safety': [
            r'SR-CL-\d+',      # Safety requirements
            r'FSR-\d+',        # Functional safety requirements
            r'TSR-\d+',        # Technical safety requirements
            r'SG-\d+',         # Safety goals
        ],
        'security': [
            r'CR-INF-\d+',     # Security requirements
            r'CSG-\d+',        # Cybersecurity goals
            r'TARA-\d+',       # Threat scenarios
        ]
    }

    # Patterns for test case IDs
    TEST_PATTERNS = [
        r'T-CL-\d+',          # Cluster tests
        r'T-FSR-\d+',         # FSR tests
        r'T-TSR-\d+',         # TSR tests
        r'SEC-\d+',           # Security tests
        r'FI-\d+',            # Fault injection tests
    ]

    def __init__(self, project_root: Path):
        self.project_root = project_root
        self.requirements: Dict[str, Requirement] = {}
        self.implementations: List[Implementation] = []
        self.tests: Dict[str, TestCase] = {}

    def analyze(self):
        """Run full analysis."""
        self._scan_documentation()
        self._scan_source_code()
        self._scan_tests()
        self._link_traceability()

    def _scan_documentation(self):
        """Scan documentation for requirement definitions."""
        docs_dir = self.project_root / "docs"
        if not docs_dir.exists():
            return

        for doc_file in docs_dir.rglob("*.md"):
            self._parse_doc_file(doc_file)

    def _parse_doc_file(self, filepath: Path):
        """Parse a documentation file for requirements."""
        try:
            content = filepath.read_text()
            lines = content.split('\n')

            category = 'safety' if 'safety' in str(filepath) else 'security'

            for i, line in enumerate(lines, 1):
                # Look for requirement definitions (tables or headers)
                for pattern_list in self.REQ_PATTERNS.values():
                    for pattern in pattern_list:
                        matches = re.findall(pattern, line)
                        for match in matches:
                            if match not in self.requirements:
                                # Extract title from context
                                title = self._extract_title(lines, i - 1)
                                self.requirements[match] = Requirement(
                                    id=match,
                                    title=title,
                                    category=category,
                                    source_file=str(filepath.relative_to(self.project_root)),
                                    source_line=i
                                )
        except Exception as e:
            print(f"Warning: Could not parse {filepath}: {e}", file=sys.stderr)

    def _extract_title(self, lines: List[str], line_idx: int) -> str:
        """Extract requirement title from surrounding context."""
        if line_idx < len(lines):
            line = lines[line_idx]
            # Remove markdown formatting
            title = re.sub(r'[|#*`]', '', line).strip()
            # Truncate if too long
            if len(title) > 100:
                title = title[:97] + "..."
            return title
        return "Unknown"

    def _scan_source_code(self):
        """Scan source code for requirement references."""
        source_dirs = [
            self.project_root / "shared",
            self.project_root / "driver_ui",
            self.project_root / "infotainment_ui",
        ]

        for source_dir in source_dirs:
            if not source_dir.exists():
                continue

            for ext in ['*.cpp', '*.h', '*.hpp', '*.qml']:
                for source_file in source_dir.rglob(ext):
                    self._parse_source_file(source_file)

    def _parse_source_file(self, filepath: Path):
        """Parse a source file for requirement references."""
        try:
            content = filepath.read_text()
            lines = content.split('\n')

            for i, line in enumerate(lines, 1):
                req_ids = []

                # Find requirement references in comments
                for category, pattern_list in self.REQ_PATTERNS.items():
                    for pattern in pattern_list:
                        matches = re.findall(pattern, line)
                        req_ids.extend(matches)

                if req_ids:
                    # Extract function name
                    func_name = self._extract_function_name(lines, i - 1)

                    impl = Implementation(
                        file=str(filepath.relative_to(self.project_root)),
                        line=i,
                        function=func_name,
                        requirement_ids=req_ids
                    )
                    self.implementations.append(impl)

                    # Update requirements with implementations
                    for req_id in req_ids:
                        if req_id in self.requirements:
                            impl_ref = f"{impl.file}:{impl.line}"
                            if impl_ref not in self.requirements[req_id].implementations:
                                self.requirements[req_id].implementations.append(impl_ref)

        except Exception as e:
            print(f"Warning: Could not parse {filepath}: {e}", file=sys.stderr)

    def _extract_function_name(self, lines: List[str], line_idx: int) -> str:
        """Extract function name from surrounding context."""
        # Look backwards for function definition
        for i in range(line_idx, max(0, line_idx - 20), -1):
            line = lines[i]
            # C++ function pattern
            match = re.search(r'(\w+::\w+|\w+)\s*\([^)]*\)\s*{?', line)
            if match:
                return match.group(1)
            # QML component pattern
            match = re.search(r'(component\s+\w+|\w+\s*{)', line)
            if match:
                return match.group(1).strip()
        return "unknown"

    def _scan_tests(self):
        """Scan test files for test cases."""
        test_dirs = [
            self.project_root / "tests",
            self.project_root / "driver_ui" / "tests",
            self.project_root / "infotainment_ui" / "tests",
        ]

        for test_dir in test_dirs:
            if not test_dir.exists():
                continue

            for test_file in test_dir.rglob("*.cpp"):
                self._parse_test_file(test_file)

        # Also check documentation for test definitions
        docs_dir = self.project_root / "docs"
        if docs_dir.exists():
            for doc_file in docs_dir.rglob("*Plan*.md"):
                self._parse_test_doc(doc_file)

    def _parse_test_file(self, filepath: Path):
        """Parse a test file for test cases."""
        try:
            content = filepath.read_text()
            lines = content.split('\n')

            for i, line in enumerate(lines, 1):
                # Find test case definitions
                for pattern in self.TEST_PATTERNS:
                    matches = re.findall(pattern, line)
                    for match in matches:
                        if match not in self.tests:
                            self.tests[match] = TestCase(
                                id=match,
                                description=self._extract_test_description(lines, i - 1)
                            )

                        # Find requirement references
                        for category, pattern_list in self.REQ_PATTERNS.items():
                            for req_pattern in pattern_list:
                                req_matches = re.findall(req_pattern, line)
                                for req_id in req_matches:
                                    if req_id not in self.tests[match].requirement_ids:
                                        self.tests[match].requirement_ids.append(req_id)
                                    if req_id in self.requirements:
                                        if match not in self.requirements[req_id].tests:
                                            self.requirements[req_id].tests.append(match)

        except Exception as e:
            print(f"Warning: Could not parse {filepath}: {e}", file=sys.stderr)

    def _parse_test_doc(self, filepath: Path):
        """Parse verification plan documentation for test cases."""
        try:
            content = filepath.read_text()

            for pattern in self.TEST_PATTERNS:
                matches = re.findall(pattern, content)
                for match in matches:
                    if match not in self.tests:
                        self.tests[match] = TestCase(
                            id=match,
                            description="Defined in documentation"
                        )

            # Link tests to requirements
            lines = content.split('\n')
            for line in lines:
                test_ids = []
                req_ids = []

                for pattern in self.TEST_PATTERNS:
                    test_ids.extend(re.findall(pattern, line))

                for category, pattern_list in self.REQ_PATTERNS.items():
                    for req_pattern in pattern_list:
                        req_ids.extend(re.findall(req_pattern, line))

                for test_id in test_ids:
                    for req_id in req_ids:
                        if test_id in self.tests:
                            if req_id not in self.tests[test_id].requirement_ids:
                                self.tests[test_id].requirement_ids.append(req_id)
                        if req_id in self.requirements:
                            if test_id not in self.requirements[req_id].tests:
                                self.requirements[req_id].tests.append(test_id)

        except Exception as e:
            print(f"Warning: Could not parse {filepath}: {e}", file=sys.stderr)

    def _extract_test_description(self, lines: List[str], line_idx: int) -> str:
        """Extract test description from surrounding context."""
        # Look for TEST_F or TEST macro
        for i in range(max(0, line_idx - 5), min(len(lines), line_idx + 5)):
            line = lines[i]
            match = re.search(r'TEST[_F]?\s*\(\s*\w+\s*,\s*(\w+)\s*\)', line)
            if match:
                return match.group(1)
        return "Unknown test"

    def _link_traceability(self):
        """Create bidirectional links between items."""
        # Update requirement status based on coverage
        for req_id, req in self.requirements.items():
            if req.implementations and req.tests:
                req.status = "verified"
            elif req.implementations:
                req.status = "implemented"
            elif req.tests:
                req.status = "test-only"
            else:
                req.status = "unverified"

    def get_statistics(self) -> Dict:
        """Calculate coverage statistics."""
        total_reqs = len(self.requirements)
        safety_reqs = sum(1 for r in self.requirements.values() if r.category == 'safety')
        security_reqs = sum(1 for r in self.requirements.values() if r.category == 'security')

        verified = sum(1 for r in self.requirements.values() if r.status == 'verified')
        implemented = sum(1 for r in self.requirements.values() if r.status in ['verified', 'implemented'])
        tested = sum(1 for r in self.requirements.values() if len(r.tests) > 0)

        return {
            'total_requirements': total_reqs,
            'safety_requirements': safety_reqs,
            'security_requirements': security_reqs,
            'total_implementations': len(self.implementations),
            'total_tests': len(self.tests),
            'verified_requirements': verified,
            'implemented_requirements': implemented,
            'tested_requirements': tested,
            'verification_coverage': verified / total_reqs * 100 if total_reqs > 0 else 0,
            'implementation_coverage': implemented / total_reqs * 100 if total_reqs > 0 else 0,
            'test_coverage': tested / total_reqs * 100 if total_reqs > 0 else 0,
        }


class ReportGenerator:
    """Generates traceability reports in various formats."""

    def __init__(self, analyzer: TraceabilityAnalyzer):
        self.analyzer = analyzer

    def generate_json(self, output_path: Path):
        """Generate JSON report."""
        report = {
            'generated_at': datetime.now().isoformat(),
            'statistics': self.analyzer.get_statistics(),
            'requirements': {
                req_id: {
                    'id': req.id,
                    'title': req.title,
                    'category': req.category,
                    'source': f"{req.source_file}:{req.source_line}",
                    'status': req.status,
                    'implementations': req.implementations,
                    'tests': req.tests,
                }
                for req_id, req in self.analyzer.requirements.items()
            },
            'tests': {
                test_id: {
                    'id': test.id,
                    'description': test.description,
                    'requirements': test.requirement_ids,
                    'result': test.result,
                }
                for test_id, test in self.analyzer.tests.items()
            }
        }

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)

    def generate_markdown(self, output_path: Path):
        """Generate Markdown report."""
        stats = self.analyzer.get_statistics()

        lines = [
            "# Traceability Report",
            "",
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            "",
            "## Summary Statistics",
            "",
            f"| Metric | Value |",
            f"|--------|-------|",
            f"| Total Requirements | {stats['total_requirements']} |",
            f"| Safety Requirements | {stats['safety_requirements']} |",
            f"| Security Requirements | {stats['security_requirements']} |",
            f"| Implementation Coverage | {stats['implementation_coverage']:.1f}% |",
            f"| Test Coverage | {stats['test_coverage']:.1f}% |",
            f"| Verification Coverage | {stats['verification_coverage']:.1f}% |",
            "",
            "## Requirements Traceability Matrix",
            "",
            "### Safety Requirements",
            "",
            "| Req ID | Status | Implementations | Tests |",
            "|--------|--------|-----------------|-------|",
        ]

        for req_id, req in sorted(self.analyzer.requirements.items()):
            if req.category == 'safety':
                impl_count = len(req.implementations)
                test_count = len(req.tests)
                status_icon = {
                    'verified': '✓',
                    'implemented': '◐',
                    'test-only': '○',
                    'unverified': '✗'
                }.get(req.status, '?')
                lines.append(f"| {req_id} | {status_icon} {req.status} | {impl_count} | {test_count} |")

        lines.extend([
            "",
            "### Security Requirements",
            "",
            "| Req ID | Status | Implementations | Tests |",
            "|--------|--------|-----------------|-------|",
        ])

        for req_id, req in sorted(self.analyzer.requirements.items()):
            if req.category == 'security':
                impl_count = len(req.implementations)
                test_count = len(req.tests)
                status_icon = {
                    'verified': '✓',
                    'implemented': '◐',
                    'test-only': '○',
                    'unverified': '✗'
                }.get(req.status, '?')
                lines.append(f"| {req_id} | {status_icon} {req.status} | {impl_count} | {test_count} |")

        lines.extend([
            "",
            "## Unverified Requirements",
            "",
        ])

        unverified = [r for r in self.analyzer.requirements.values() if r.status == 'unverified']
        if unverified:
            for req in unverified:
                lines.append(f"- **{req.id}**: {req.title}")
        else:
            lines.append("None - all requirements are at least partially traced.")

        lines.extend([
            "",
            "## Test Coverage",
            "",
            "| Test ID | Requirements Covered |",
            "|---------|---------------------|",
        ])

        for test_id, test in sorted(self.analyzer.tests.items()):
            reqs = ", ".join(test.requirement_ids) if test.requirement_ids else "None"
            lines.append(f"| {test_id} | {reqs} |")

        lines.extend([
            "",
            "---",
            "*Report generated by traceability tool*",
        ])

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            f.write('\n'.join(lines))

    def generate_html(self, output_path: Path):
        """Generate HTML report."""
        stats = self.analyzer.get_statistics()

        html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Traceability Report</title>
    <style>
        body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 40px; background: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background: white; padding: 40px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }}
        h1 {{ color: #1a1a2e; border-bottom: 2px solid #4a4a8a; padding-bottom: 10px; }}
        h2 {{ color: #4a4a8a; margin-top: 30px; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background: #4a4a8a; color: white; }}
        tr:hover {{ background: #f5f5f5; }}
        .stat-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }}
        .stat-card {{ background: linear-gradient(135deg, #4a4a8a, #6a6aaa); color: white; padding: 20px; border-radius: 8px; text-align: center; }}
        .stat-value {{ font-size: 2em; font-weight: bold; }}
        .stat-label {{ opacity: 0.9; margin-top: 5px; }}
        .status-verified {{ color: #22c55e; }}
        .status-implemented {{ color: #60a5fa; }}
        .status-test-only {{ color: #f59e0b; }}
        .status-unverified {{ color: #ef4444; }}
        .progress-bar {{ background: #e0e0e0; border-radius: 10px; overflow: hidden; height: 20px; }}
        .progress-fill {{ height: 100%; background: linear-gradient(90deg, #22c55e, #60a5fa); }}
        .timestamp {{ color: #666; font-size: 0.9em; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Traceability Report</h1>
        <p class="timestamp">Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>

        <h2>Coverage Overview</h2>
        <div class="stat-grid">
            <div class="stat-card">
                <div class="stat-value">{stats['total_requirements']}</div>
                <div class="stat-label">Total Requirements</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{stats['verification_coverage']:.1f}%</div>
                <div class="stat-label">Verification Coverage</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{stats['implementation_coverage']:.1f}%</div>
                <div class="stat-label">Implementation Coverage</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{stats['test_coverage']:.1f}%</div>
                <div class="stat-label">Test Coverage</div>
            </div>
        </div>

        <h2>Coverage Progress</h2>
        <div class="progress-bar">
            <div class="progress-fill" style="width: {stats['verification_coverage']}%"></div>
        </div>

        <h2>Safety Requirements</h2>
        <table>
            <tr>
                <th>Req ID</th>
                <th>Title</th>
                <th>Status</th>
                <th>Implementations</th>
                <th>Tests</th>
            </tr>
"""

        for req_id, req in sorted(self.analyzer.requirements.items()):
            if req.category == 'safety':
                status_class = f"status-{req.status}"
                html += f"""            <tr>
                <td><strong>{req_id}</strong></td>
                <td>{req.title[:50]}...</td>
                <td class="{status_class}">{req.status}</td>
                <td>{len(req.implementations)}</td>
                <td>{len(req.tests)}</td>
            </tr>
"""

        html += """        </table>

        <h2>Security Requirements</h2>
        <table>
            <tr>
                <th>Req ID</th>
                <th>Title</th>
                <th>Status</th>
                <th>Implementations</th>
                <th>Tests</th>
            </tr>
"""

        for req_id, req in sorted(self.analyzer.requirements.items()):
            if req.category == 'security':
                status_class = f"status-{req.status}"
                html += f"""            <tr>
                <td><strong>{req_id}</strong></td>
                <td>{req.title[:50]}...</td>
                <td class="{status_class}">{req.status}</td>
                <td>{len(req.implementations)}</td>
                <td>{len(req.tests)}</td>
            </tr>
"""

        html += """        </table>
    </div>
</body>
</html>
"""

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(html)


def main():
    parser = argparse.ArgumentParser(description='Generate traceability reports')
    parser.add_argument('--format', choices=['json', 'markdown', 'html', 'all'],
                        default='all', help='Output format')
    parser.add_argument('--output', type=Path, default=Path('reports/traceability'),
                        help='Output directory')
    parser.add_argument('--project', type=Path, default=Path('.'),
                        help='Project root directory')

    args = parser.parse_args()

    print(f"Analyzing project: {args.project.absolute()}")

    analyzer = TraceabilityAnalyzer(args.project.absolute())
    analyzer.analyze()

    stats = analyzer.get_statistics()
    print(f"\nFound {stats['total_requirements']} requirements")
    print(f"  - Safety: {stats['safety_requirements']}")
    print(f"  - Security: {stats['security_requirements']}")
    print(f"  - Verification coverage: {stats['verification_coverage']:.1f}%")

    generator = ReportGenerator(analyzer)

    if args.format in ['json', 'all']:
        output_file = args.output / 'traceability.json'
        generator.generate_json(output_file)
        print(f"Generated: {output_file}")

    if args.format in ['markdown', 'all']:
        output_file = args.output / 'traceability.md'
        generator.generate_markdown(output_file)
        print(f"Generated: {output_file}")

    if args.format in ['html', 'all']:
        output_file = args.output / 'traceability.html'
        generator.generate_html(output_file)
        print(f"Generated: {output_file}")

    # Return non-zero if coverage is below threshold
    if stats['verification_coverage'] < 50:
        print(f"\nWarning: Verification coverage ({stats['verification_coverage']:.1f}%) is below 50%")
        return 1

    return 0


if __name__ == '__main__':
    sys.exit(main())
