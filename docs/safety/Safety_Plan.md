# Safety Plan (SAFE-PLAN-001)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-PLAN-001

---

## 1. Safety Lifecycle Scope

### 1.1 Item Definition Boundary

| Component | Safety Classification | Rationale |
|-----------|----------------------|-----------|
| Driver UI (Cluster) | Safety-Related (ASIL QM to ASIL B per function) | Displays speed, warnings, telltales to driver |
| Infotainment UI | Non-Safety (QM) | Does not present safety-critical information |
| Shared Signal Layer | Safety-Related Support | Provides validated signals to Driver UI |
| Shared IPC Layer | QM with security focus | Communication backbone |

### 1.2 Assumptions

1. **ASM-001**: Vehicle signal truth originates from certified ECU(s) with appropriate ASIL ratings
2. **ASM-002**: UI does not command torque, braking, or steering in v1
3. **ASM-003**: UI must not mislead driver regarding vehicle state
4. **ASM-004**: Display hardware meets automotive qualification requirements
5. **ASM-005**: GPU driver stability is validated for target platform
6. **ASM-006**: Operating system provides deterministic scheduling capability

### 1.3 Out of Scope

- Hardware qualification (covered by separate process)
- ECU signal generation and validation (upstream responsibility)
- Physical display characteristics (brightness, viewing angle)
- Audio subsystem safety requirements

---

## 2. Roles and Responsibilities

| Role | Responsibilities | Qualification |
|------|------------------|---------------|
| Safety Manager | Overall safety lifecycle ownership, milestone approvals | ISO 26262 trained |
| Safety Engineer | HARA, FSC, TSC development, requirement authoring | ISO 26262 certified |
| SOTIF Engineer | SOTIF scenario analysis, limitation identification | ISO 21448 trained |
| Independent Reviewer | Confirmation reviews, safety case audit | Independent from development |
| Tool Owner | Tool qualification evidence, TCL assessment | Tool qualification experience |
| Software Architect | Safety architecture compliance | ASPICE SWE trained |
| Test Engineer | Safety test execution, evidence collection | Automotive test experience |

---

## 3. Tools and Qualification

### 3.1 Development Tools

| Tool | Purpose | TCL | Qualification Approach |
|------|---------|-----|------------------------|
| GCC/Clang | C++ compilation | TCL3 | Increased confidence from use |
| Qt 6.5+ | UI framework | TCL2 | Qualification test suite |
| CMake | Build system | TCL3 | Increased confidence from use |
| clang-tidy | Static analysis | TCL2 | Validation against known defects |
| qmllint | QML validation | TCL2 | Validation test suite |

### 3.2 Verification Tools

| Tool | Purpose | TCL | Qualification Approach |
|------|---------|-----|------------------------|
| Google Test | Unit testing | TCL1 | Tool qualification report |
| Fault Injector | Safety verification | TCL1 | Custom qualification |
| Coverage tools | Code coverage | TCL2 | Validation against known code |

### 3.3 Tool Confidence Level Notes

- **TCL1**: Tool can introduce errors; requires qualification
- **TCL2**: Tool may fail to detect errors; requires validation
- **TCL3**: Tool unlikely to affect safety; confidence from use

---

## 4. Safety Milestones

| Milestone ID | Description | Entry Criteria | Exit Criteria | Deliverables |
|--------------|-------------|----------------|---------------|--------------|
| SAFE-MS-001 | Item Definition Complete | Project kickoff | Review approval | SAFE-ITEM-001 |
| SAFE-MS-002 | HARA Complete | Item definition | ASIL ratings confirmed | HARA document |
| SAFE-MS-003 | Safety Requirements Baseline | HARA complete | Requirements review | FSC, TSC documents |
| SAFE-MS-004 | Verification Plan Baseline | Requirements baseline | Plan review | SAFE-VER-001 |
| SAFE-MS-005 | SOTIF Analysis Complete | Item definition | Scenario review | SOTIF document |
| SAFE-MS-006 | Safety Case Draft | All verification complete | Independent review | SAFE-CASE-001 |

---

## 5. Configuration Management

### 5.1 Safety Work Products

All safety work products shall be:
- Version controlled in the project repository
- Traceable to requirements via document IDs
- Subject to change control process
- Reviewed before release

### 5.2 Document Naming Convention

```
docs/safety/<DocumentType>_<Version>.md
```

### 5.3 Baseline Management

Safety baselines established at each milestone:
- SAFE-BL-001: Requirements baseline
- SAFE-BL-002: Design baseline
- SAFE-BL-003: Verification baseline

---

## 6. Tailoring Decisions

| ISO 26262 Clause | Tailoring | Rationale |
|------------------|-----------|-----------|
| Part 5 (Hardware) | Out of scope | Software-only deliverable |
| Part 8 (Supporting) | Partial | Desktop development environment |
| Part 11 (Semiconductors) | Out of scope | Uses qualified components |

---

## 7. References

- ISO 26262:2018 Road vehicles - Functional safety
- ISO/PAS 21448:2022 SOTIF
- ASPICE v3.1 Process Reference Model
- Project Traceability Matrix (tools/traceability/)

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
