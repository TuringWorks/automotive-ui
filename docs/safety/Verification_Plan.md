# Verification Plan (SAFE-VER-001)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-VER-001

---

## 1. Introduction

### 1.1 Purpose

This document defines the verification plan for the Driver UI safety requirements, specifying test methods, pass criteria, and evidence artifacts for ISO 26262 compliance.

### 1.2 Scope

Covers verification of:
- Functional Safety Requirements (FSRs)
- Technical Safety Requirements (TSRs)
- Safety mechanisms and monitoring
- Degraded mode behavior

---

## 2. Verification Methods

### 2.1 Method Overview

| Method | Code | Description | Application |
|--------|------|-------------|-------------|
| Review | R | Inspection of work products | Requirements, design, code |
| Analysis | A | Static/dynamic analysis | Timing, memory, coverage |
| Test | T | Execution-based verification | Unit, integration, system |
| Fault Injection | FI | Deliberate fault introduction | Safety mechanism validation |

### 2.2 Method Selection by ASIL

| ASIL | Review | Analysis | Test | Fault Injection |
|------|--------|----------|------|-----------------|
| QM | Recommended | Optional | Required | Optional |
| ASIL A | Required | Recommended | Required | Recommended |
| ASIL B | Required | Required | Required | Required |
| ASIL C | Required | Required | Required | Required |

---

## 3. Test Environment

### 3.1 Desktop Test Environment

| Component | Tool/Version | Purpose |
|-----------|--------------|---------|
| Compiler | GCC 11+ / Clang 14+ | Code compilation |
| Test Framework | Google Test 1.12+ | Unit test execution |
| Coverage | gcov/lcov or llvm-cov | Code coverage analysis |
| Static Analysis | clang-tidy with MISRA rules | Static defect detection |
| QML Lint | qmllint (Qt 6.5+) | QML validation |
| Fault Injector | Custom FaultInjector class | Safety mechanism testing |

### 3.2 Simulation Environment

| Component | Description | Purpose |
|-----------|-------------|---------|
| Signal Simulator | Generates test signals | Input simulation |
| Timing Simulator | Adjustable time source | Staleness testing |
| Fault Injector | Injects various faults | Safety verification |
| Display Capture | Screenshot/video capture | Visual verification |

---

## 4. Verification Matrix

### 4.1 Safety Requirements Verification

| Req ID | Requirement Summary | Method | Test ID(s) | Pass Criteria | Evidence |
|--------|---------------------|--------|------------|---------------|----------|
| SR-CL-001 | Speed ≥10Hz, stale ≤300ms | T, FI | T-CL-001, T-CL-002 | Update rate verified, staleness detected | test_report.json |
| SR-CL-002 | Clamp invalid, show indicator | T, FI | T-CL-003, T-CL-004 | Invalid values clamped, indicator visible | test_report.json |
| SR-CL-003 | Critical telltales visible | T, R | T-CL-005 | Telltales never obscured | visual_test.png |
| SR-CL-004 | Degraded mode on signal loss | T, FI | T-CL-006, T-CL-007 | Correct mode transitions | test_report.json |

### 4.2 Functional Safety Requirements Verification

| FSR ID | Requirement | Method | Test ID(s) | Pass Criteria |
|--------|-------------|--------|------------|---------------|
| FSR-001 | Signal validation (range, rate, freshness) | T, A | T-FSR-001a/b/c | All checks functional |
| FSR-002 | Invalid indication within 100ms | T | T-FSR-002 | Latency ≤100ms |
| FSR-003 | Telltale priority and visibility | T, R | T-FSR-003 | Priority enforced |
| FSR-004 | ADAS default inactive | T, FI | T-FSR-004 | Correct default state |
| FSR-005 | Frame health monitoring | T, FI | T-FSR-005 | Degraded mode on failure |
| FSR-006 | Gear validation | T | T-FSR-006 | Unknown state displayed |

### 4.3 Technical Safety Requirements Verification

| TSR ID | Requirement | Method | Test ID(s) | Pass Criteria |
|--------|-------------|--------|------------|---------------|
| TSR-001 | Speed range 0-300 km/h | T | T-TSR-001 | Values outside range rejected |
| TSR-002 | Rate-of-change ≤15 m/s² | T | T-TSR-002 | Excessive rate rejected |
| TSR-003 | Freshness ≤300ms | T, FI | T-TSR-003 | Stale detection functional |
| TSR-004 | Invalid display "---" in 100ms | T | T-TSR-004 | Display updates correctly |
| TSR-005 | Stale indicator visible | T | T-TSR-005 | Visual indicator present |
| TSR-006 | Telltale z-order | R, T | T-TSR-006 | Z-order correct |
| TSR-007 | Alert doesn't obscure telltales | T | T-TSR-007 | Layout verified |
| TSR-008 | ADAS default inactive | T | T-TSR-008 | Initial state correct |
| TSR-009 | Frame heartbeat 60Hz | A, T | T-TSR-009 | Timing verified |
| TSR-010 | Degraded after 10 missed frames | FI | T-TSR-010 | Transition correct |
| TSR-011 | Invalid gear shows "?" | T | T-TSR-011 | Display correct |

---

## 5. Test Cases

### 5.1 Signal Validation Tests

#### T-CL-001: Speed Update Rate
```
Test ID: T-CL-001
Requirement: SR-CL-001
Method: Test
Preconditions:
  - System initialized
  - Signal simulator running
Steps:
  1. Send speed signals at 10Hz for 10 seconds
  2. Record update timestamps
  3. Calculate update rate
Pass Criteria:
  - Measured update rate ≥10Hz
  - No gaps >100ms between updates
Evidence: test_reports/T-CL-001.json
```

#### T-CL-002: Staleness Detection
```
Test ID: T-CL-002
Requirement: SR-CL-001
Method: Test, Fault Injection
Preconditions:
  - System initialized
  - Valid speed signal established
Steps:
  1. Send valid speed signal
  2. Stop signal updates (inject fault)
  3. Wait 300ms
  4. Verify stale indicator appears
  5. Wait additional 200ms
  6. Verify stale state maintained
Pass Criteria:
  - Stale indicator appears within 300ms of last update
  - Speed value retained during stale state
Evidence: test_reports/T-CL-002.json
```

#### T-CL-003: Out of Range Handling
```
Test ID: T-CL-003
Requirement: SR-CL-002
Method: Test
Preconditions:
  - System initialized
Steps:
  1. Send speed = -10 km/h
  2. Verify clamped to 0 and invalid shown
  3. Send speed = 350 km/h
  4. Verify clamped to 300 and invalid shown
  5. Send speed = 150 km/h
  6. Verify displayed correctly as valid
Pass Criteria:
  - Negative values clamped to 0
  - Values >300 clamped to 300
  - Invalid indicator shown for out-of-range
  - Valid values displayed normally
Evidence: test_reports/T-CL-003.json
```

#### T-CL-004: Invalid Signal Indication
```
Test ID: T-CL-004
Requirement: SR-CL-002
Method: Test
Preconditions:
  - System initialized
  - Valid speed displayed
Steps:
  1. Mark speed signal as invalid
  2. Measure time until "---" displayed
  3. Verify invalid indicator visible
Pass Criteria:
  - "---" displayed within 100ms
  - Invalid indicator visible
Evidence: test_reports/T-CL-004.json
```

### 5.2 Telltale and Alert Tests

#### T-CL-005: Critical Telltale Visibility
```
Test ID: T-CL-005
Requirement: SR-CL-003
Method: Test, Review
Preconditions:
  - System initialized
Steps:
  1. Activate critical telltale (e.g., brake warning)
  2. Display multiple alerts
  3. Verify telltale bar visible
  4. Capture screenshot
  5. Review telltale visibility
Pass Criteria:
  - Critical telltale visible in all conditions
  - Never obscured by alerts or content
Evidence: test_reports/T-CL-005.json, screenshots/telltale_visibility.png
```

### 5.3 Degraded Mode Tests

#### T-CL-006: Degraded Mode Entry
```
Test ID: T-CL-006
Requirement: SR-CL-004
Method: Test, Fault Injection
Preconditions:
  - System initialized
  - Normal operation state
Steps:
  1. Inject fault: make 3 critical signals stale
  2. Measure time until degraded mode
  3. Verify degraded UI displayed
  4. Clear faults
  5. Verify recovery to normal
Pass Criteria:
  - Degraded mode entered within 500ms
  - Correct degraded UI displayed
  - Recovery to normal when signals valid for 5s
Evidence: test_reports/T-CL-006.json
```

#### T-CL-007: Minimal Mode Entry
```
Test ID: T-CL-007
Requirement: SR-CL-004
Method: Test, Fault Injection
Preconditions:
  - System initialized
Steps:
  1. Inject fault: complete signal loss
  2. Verify minimal mode entered
  3. Verify fault message displayed
  4. Restore signals
  5. Verify recovery
Pass Criteria:
  - Minimal mode entered on complete signal loss
  - Fault message clearly displayed
  - System recovers when signals return
Evidence: test_reports/T-CL-007.json
```

---

## 6. Analysis Activities

### 6.1 Static Analysis

| Analysis ID | Target | Tool | Rules | Criteria |
|-------------|--------|------|-------|----------|
| SA-001 | Safety Core C++ | clang-tidy | MISRA-inspired subset | No high-severity findings |
| SA-002 | All C++ | clang-tidy | bugprone, performance | No critical findings |
| SA-003 | QML files | qmllint | All enabled | No errors |

### 6.2 Code Coverage

| Coverage ID | Target | Metric | Criteria |
|-------------|--------|--------|----------|
| COV-001 | Safety Core | Statement | ≥90% |
| COV-002 | Safety Core | Branch | ≥80% |
| COV-003 | Signal validation | MC/DC | ≥95% |

### 6.3 Timing Analysis

| Analysis ID | Target | Metric | Criteria |
|-------------|--------|--------|----------|
| TIM-001 | Signal processing | WCET | ≤10ms |
| TIM-002 | Staleness check | Latency | ≤5ms |
| TIM-003 | Display update | Frame time | ≤16.7ms |

---

## 7. Fault Injection Test Scenarios

### 7.1 Scenario Catalog

| Scenario ID | Fault Type | Injection Method | Expected Behavior |
|-------------|------------|------------------|-------------------|
| FI-001 | Stale speed signal | Stop signal updates | Stale indicator shown |
| FI-002 | Out of range speed | Send value >300 | Clamped + invalid |
| FI-003 | Multiple stale signals | Stop 3+ signals | Degraded mode |
| FI-004 | Complete signal loss | Stop all signals | Minimal mode |
| FI-005 | Frame render stall | Block render callback | Watchdog triggers |
| FI-006 | Rapid value changes | Oscillate values | Rate-of-change rejection |

### 7.2 Fault Injection API Usage

```cpp
// Example fault injection test
TEST_F(SafetyTest, StaleSpeedShowsIndicator) {
    // Setup
    system->initialize();
    sendValidSpeed(100.0);

    // Inject fault
    faultInjector->injectFault(FaultScenario::StaleSpeed);

    // Wait for staleness threshold
    advanceTime(350);  // >300ms

    // Verify
    EXPECT_TRUE(clusterModel->speedStale());
    EXPECT_TRUE(isStaleIndicatorVisible());

    // Cleanup
    faultInjector->clearAllFaults();
}
```

---

## 8. Review Activities

### 8.1 Review Checklist

| Review ID | Work Product | Checklist Items |
|-----------|--------------|-----------------|
| REV-001 | Requirements | Completeness, consistency, testability |
| REV-002 | Architecture | Safety partitioning, interface definitions |
| REV-003 | Design | Compliance with FSC/TSC |
| REV-004 | Code | Coding standards, safety patterns |
| REV-005 | Tests | Coverage, traceability |

### 8.2 Review Records

| Review ID | Date | Participants | Findings | Status |
|-----------|------|--------------|----------|--------|
| REV-001 | TBD | | | Pending |
| REV-002 | TBD | | | Pending |
| REV-003 | TBD | | | Pending |
| REV-004 | TBD | | | Pending |
| REV-005 | TBD | | | Pending |

---

## 9. Test Execution Schedule

### 9.1 Test Phases

| Phase | Scope | Entry Criteria | Exit Criteria |
|-------|-------|----------------|---------------|
| Unit Test | Individual components | Code complete | Coverage met |
| Integration Test | Component interactions | Unit tests pass | Interface tests pass |
| System Test | Full system behavior | Integration pass | All requirements verified |
| Fault Injection | Safety mechanisms | System tests pass | All scenarios pass |

### 9.2 CI/CD Integration

Tests executed automatically on:
- Every commit (unit tests)
- Every PR (unit + integration)
- Nightly (full suite including fault injection)
- Release (all tests + manual review)

---

## 10. Evidence Artifacts

### 10.1 Artifact Catalog

| Artifact ID | Description | Location | Format |
|-------------|-------------|----------|--------|
| EV-001 | Unit test results | reports/test/unit/ | JSON |
| EV-002 | Integration test results | reports/test/integration/ | JSON |
| EV-003 | Coverage reports | reports/coverage/ | HTML/JSON |
| EV-004 | Static analysis reports | reports/analysis/ | JSON |
| EV-005 | Fault injection results | reports/test/fault_injection/ | JSON |
| EV-006 | Review records | docs/reviews/ | Markdown |
| EV-007 | Visual test screenshots | reports/screenshots/ | PNG |

### 10.2 Traceability Report

Generated by `tools/traceability/generate_report.py`:
- Requirements → Tests
- Tests → Results
- Safety Goals → Verification Evidence

---

## 11. References

- SAFE-PLAN-001: Safety Plan
- SAFE-HARA-001: Hazard Analysis
- SAFE-FSC-001: Functional Safety Concept
- SAFE-TSC-001: Technical Safety Concept

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
