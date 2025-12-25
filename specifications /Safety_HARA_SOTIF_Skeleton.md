# Safety Artifacts Skeleton (ISO 26262 + SOTIF)
Version: 0.1  
Date: 2025-12-23

This file is a **starter skeleton** intended to be filled and iterated by Safety Engineer and SOTIF Engineer agents.

---

## 1) Safety Plan (SAFE-PLAN-001)
### 1.1 Safety lifecycle scope
- Item definition boundary:
  - Driver UI (cluster) as safety-related HMI component
  - Infotainment as non-safety (but may influence safety via distraction/cyber)
- Assumptions:
  - Vehicle signal truth originates from certified ECU(s)
  - UI does not command torque/braking in v1
  - UI must not mislead driver

### 1.2 Roles & responsibilities
- Safety Manager:
- Safety Engineer:
- Independent Reviewer:
- Tool Owner:

### 1.3 Tools and qualification notes
- Compiler/toolchain:
- Static analysis:
- Unit test framework:
- Notes on tool confidence level (TCL) and qualification approach:

### 1.4 Safety milestones (example)
- SAFE-MS-001: Item definition complete
- SAFE-MS-002: HARA complete
- SAFE-MS-003: Safety requirements baseline
- SAFE-MS-004: Verification plan baseline
- SAFE-MS-005: Safety case draft

---

## 2) Item Definition (SAFE-ITEM-001)
### 2.1 Item description
- Driver UI renders speed, gear, warning telltales, and prioritized alerts.
- Driver UI indicates signal validity and stale data.
- Degraded mode behavior defined for loss of reliable signals.

### 2.2 Operational scenarios
- City driving, highway, night/rain, low visibility, low temperature
- Startup, shutdown, sleep/wake
- Partial signal loss, sensor drift, bus overload

### 2.3 Interfaces
- Inputs: validated signal set from `VehicleSignalsService`
- Outputs: visual telltales, alerts, optional audible cues (stub v1)
- External dependencies: OS, GPU driver, display HW

---

## 3) HARA (Hazard Analysis and Risk Assessment)
> Use this as the working table. Produce ASIL targets where applicable.

### 3.1 Hazard list table
| HARA ID | Operational Situation | Hazard | Malfunctioning Behavior | Severity (S) | Exposure (E) | Controllability (C) | ASIL | Safety Goal ID |
|---|---|---|---|---|---|---|---|---|
| HARA-001 | Highway, 65+ mph | Driver misjudges speed | Speed shown lower than actual | TBD | TBD | TBD | TBD | SG-001 |
| HARA-002 | City, intersections | Missed warning | Critical telltale not shown | TBD | TBD | TBD | TBD | SG-002 |
| HARA-003 | Night / rain | Incorrect ADAS state | ADAS indicates active when not | TBD | TBD | TBD | TBD | SG-003 |
| HARA-004 | Any | Stale data not indicated | UI appears valid but is stale | TBD | TBD | TBD | TBD | SG-004 |

### 3.2 Safety goals (examples)
- **SG-001:** Prevent misleading speed indication; detect and indicate invalid/stale speed.
- **SG-002:** Ensure critical telltales/alerts are always visible and prioritized.
- **SG-003:** Prevent misleading driver-assistance state representation.
- **SG-004:** Always indicate loss of integrity/freshness; enter degraded mode.

---

## 4) Functional Safety Concept (FSC) — Skeleton
### 4.1 FSC overview
- Map safety goals to functional safety requirements (FSRs).

### 4.2 Example FSRs (to be expanded)
- **FSR-001:** Speed signal shall be validated for range, rate-of-change, and freshness.
- **FSR-002:** UI shall show stale/invalid indicators within defined latency.
- **FSR-003:** Alert manager shall enforce strict priority and occlusion rules.
- **FSR-004:** Degraded mode shall reduce UI to essential information and indicate fault state.

---

## 5) Technical Safety Concept (TSC) — Skeleton
### 5.1 Architecture safety mechanisms
- Watchdog on signal update loop
- Plausibility checks (Δv/Δt bounds)
- Monotonic timebase
- Rendering health check (frame heartbeat)
- Fail-safe UI state machine

### 5.2 Hardware/OS assumptions
- Display refresh ≥ 60Hz (target) or specified minimum
- GPU driver stability requirements (to be documented)

---

## 6) Verification Plan (SAFE-VER-001)
### 6.1 Verification methods
- Review: requirements/design reviews
- Analysis: static analysis, worst-case timing (best-effort on desktop)
- Test: unit, integration, UI smoke, fault injection
- Traceability: Req → Tests → Evidence

### 6.2 Verification matrix (template)
| Requirement ID | Method | Test ID(s) | Pass Criteria | Evidence Artifact |
|---|---|---|---|---|
| SR-CL-001 | Test | T-CL-001 | ≥10Hz update; stale <300ms | `reports/test/cluster.json` |
| SR-CL-002 | Test | T-CL-002 | clamp + invalid indicator | `reports/test/cluster.json` |

---

## 7) SOTIF (ISO/PAS 21448) Scenarios
> Focus: hazards when the UI operates “as designed” but is unsafe due to limitations, ambiguity, or environment.

### 7.1 Scenario table
| SOTIF ID | Scenario | Trigger / Conditions | Potential Hazard | Mitigation Strategy | Validation |
|---|---|---|---|---|---|
| SOTIF-001 | Driver glances during vibration | Rough road, glare | Missed alert | Larger glyph + persistent critical bar | simulator test |
| SOTIF-002 | Unit confusion | km/h vs mph mismatch | Speed misinterpretation | explicit unit lock + profile binding | unit test |
| SOTIF-003 | Information overload | multiple warnings | distraction | strict alert ladder + bundling | UI test |

### 7.2 SOTIF mitigations checklist
- Reduce ambiguity (units, state labels)
- Prioritize hazards (alert discipline)
- Improve visibility (contrast, redundancy)
- Handle environmental limitations (glare/night modes)

---

## 8) Safety Case Outline (SAFE-CASE-001)
- Claim: Driver UI meets safety goals for defined item boundary.
- Evidence:
  - Requirements baseline + traceability
  - Test reports + fault injection outcomes
  - Static analysis reports
  - Reviews and approvals
