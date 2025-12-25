# SOTIF Scenarios (ISO/PAS 21448)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-SOTIF-001

---

## 1. Introduction

### 1.1 Purpose

This document identifies and analyzes Safety of the Intended Functionality (SOTIF) scenarios for the Driver UI component. SOTIF addresses hazards arising from functional insufficiencies or reasonably foreseeable misuse, even when the system operates as designed.

### 1.2 Scope

Analysis covers:
- Triggering conditions leading to potentially hazardous behavior
- Scenarios where the UI operates correctly but may still mislead
- Human factors and perception limitations
- Environmental factors affecting UI usability

### 1.3 SOTIF vs Functional Safety

| Aspect | Functional Safety (ISO 26262) | SOTIF (ISO 21448) |
|--------|------------------------------|-------------------|
| Focus | Malfunctions/failures | Intended functionality limitations |
| Cause | Hardware/software faults | Design limitations, misuse |
| Example | Speed sensor fails | Speed readable but misinterpreted |

---

## 2. Scenario Classification

### 2.1 SOTIF Area Model

```
┌───────────────────────────────────────────────────────┐
│                     Operating Conditions              │
├───────────────────────────────────────────────────────┤
│  ┌──────────────────────┐  ┌─────────────────────-─┐  │
│  │   Known Safe (A1)    │  │  Known Unsafe (A2)    │  │
│  │                      │  │                       │  │
│  │ - Normal operation   │  │ - Identified issues   │  │
│  │ - Validated scenarios│  │ - Being mitigated     │  │
│  └──────────────────────┘  └───────────────────────┘  │
│  ┌──────────────────────┐  ┌──────────────────────-┐  │
│  │ Unknown Safe (A3)    │  │ Unknown Unsafe (A4)   │  │
│  │                      │  │                       │  │
│  │ - Not yet tested     │  │ - Undiscovered issues │  │
│  │ - Assumed safe       │  │ - Target of analysis  │  │
│  └──────────────────────┘  └───────────────────────┘  │
└───────────────────────────────────────────────────────┘

Goal: Move scenarios from A4 → A2 → A1 through analysis and mitigation
```

---

## 3. Triggering Conditions Catalog

### 3.1 Environmental Triggering Conditions

| TC ID | Condition | Impact on UI | Affected Functions |
|-------|-----------|--------------|-------------------|
| TC-ENV-001 | Direct sunlight/glare | Reduced contrast | All displays |
| TC-ENV-002 | Night driving | Dark adaptation | Theme switching |
| TC-ENV-003 | Rain/condensation | Peripheral vision focus | Alert visibility |
| TC-ENV-004 | Vibration (rough road) | Display readability | Speed, gauges |
| TC-ENV-005 | Extreme cold | Display response time | All displays |
| TC-ENV-006 | Extreme heat | Display degradation | All displays |

### 3.2 Human Factors Triggering Conditions

| TC ID | Condition | Impact | Affected Functions |
|-------|-----------|--------|-------------------|
| TC-HF-001 | Brief glance | Limited information intake | Complex displays |
| TC-HF-002 | Cognitive load | Reduced attention | Multi-element displays |
| TC-HF-003 | Age-related vision | Small text/icons | Labels, numbers |
| TC-HF-004 | Color blindness | Red/green confusion | Status indicators |
| TC-HF-005 | Familiarity assumption | Missed changes | State transitions |
| TC-HF-006 | Unit confusion | Value misinterpretation | Speed, temperature |

### 3.3 Operational Triggering Conditions

| TC ID | Condition | Impact | Affected Functions |
|-------|-----------|--------|-------------------|
| TC-OP-001 | Information overload | Alert fatigue | Multiple warnings |
| TC-OP-002 | Competing attention | Missed warning | Alerts during navigation |
| TC-OP-003 | Unusual vehicle state | Unexpected readings | New vehicle features |
| TC-OP-004 | Multi-driver use | Preference mismatch | Settings, units |

---

## 4. SOTIF Scenario Analysis

### 4.1 SOTIF-001: Glance During Vibration

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-001 |
| **Description** | Driver glances at speed during vibration on rough road |
| **Triggering Conditions** | TC-ENV-004, TC-HF-001 |
| **Scenario Type** | Known Unsafe (A2) - Being mitigated |

**Situation**:
Driver traveling on rough road surface, vehicle vibrating. Driver takes brief glance at speedometer to check speed.

**Potential Hazard**:
- Speed digits blur or become unreadable
- Driver misreads speed (e.g., 85 vs 65)
- Driver continues at inappropriate speed

**Risk Assessment**:
- Severity: S2 (collision due to inappropriate speed)
- Probability: Medium (rough roads are common)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Larger font size | Speed font ≥48pt | Implemented |
| High contrast | Black on white or inverse | Implemented |
| Persistence | No rapid value flicker | Implemented |
| Stabilization | Display update smoothing | Planned |

**Validation Method**:
- User testing with simulated vibration
- Eye tracking during glance task

---

### 4.2 SOTIF-002: Unit Confusion

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-002 |
| **Description** | Driver accustomed to km/h reads mph display |
| **Triggering Conditions** | TC-HF-006, TC-OP-004 |
| **Scenario Type** | Known Unsafe (A2) - Mitigated |

**Situation**:
Driver from metric country uses vehicle configured for imperial units, or driver switches between vehicles with different settings.

**Potential Hazard**:
- Speed misinterpretation (60 mph read as 60 km/h)
- Driver travels much faster than intended
- Potential for excessive speed violations or accidents

**Risk Assessment**:
- Severity: S3 (high-speed collision)
- Probability: Low-Medium (depends on market/use case)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Explicit unit display | "mph" or "km/h" always visible | Implemented |
| Profile binding | Units tied to driver profile | Implemented |
| Startup reminder | Brief unit confirmation on start | Planned |
| Dual display option | Both units shown | Future |

**Validation Method**:
- User comprehension testing
- Post-drive surveys on unit awareness

---

### 4.3 SOTIF-003: Information Overload

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-003 |
| **Description** | Multiple simultaneous warnings cause driver distraction |
| **Triggering Conditions** | TC-OP-001, TC-HF-002 |
| **Scenario Type** | Known Unsafe (A2) - Being mitigated |

**Situation**:
Vehicle generates multiple warnings simultaneously (low fuel, door ajar, lane departure, collision warning), overwhelming the driver.

**Potential Hazard**:
- Driver distracted trying to process warnings
- Critical warning missed among less critical ones
- Delayed response to genuine emergency
- Alert fatigue leading to ignored warnings

**Risk Assessment**:
- Severity: S3 (distraction leading to collision)
- Probability: Medium (complex vehicles have many warnings)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Strict priority ladder | 4 priority levels | Implemented |
| Alert bundling | Group related alerts | Implemented |
| Sequential presentation | One alert at a time for non-critical | Implemented |
| Critical always shown | P0 alerts never queued | Implemented |
| Audio differentiation | Different sounds for severity | Planned |

**Validation Method**:
- Cognitive load testing
- Simulated multi-alert scenarios

---

### 4.4 SOTIF-004: Stale Data Trust

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-004 |
| **Description** | Driver trusts displayed value that is actually stale |
| **Triggering Conditions** | TC-HF-005 |
| **Scenario Type** | Known Safe (A1) - Mitigated |

**Situation**:
Signal becomes stale due to CAN bus issue, but driver continues to trust displayed speed because it "looks normal".

**Potential Hazard**:
- Driver makes decisions based on outdated information
- Speed could have changed significantly

**Risk Assessment**:
- Severity: S2-S3 (depends on speed delta)
- Probability: Low (staleness is rare)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Visual stale indicator | Dimming + icon | Implemented |
| Timestamp visibility | "Last update X sec ago" | Optional |
| Degraded mode entry | Clear state change | Implemented |
| No "stuck valid" state | Continuous freshness check | Implemented |

**Validation Method**:
- Fault injection testing
- User recognition testing for stale indicators

---

### 4.5 SOTIF-005: Color Vision Deficiency

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-005 |
| **Description** | Driver with red-green color blindness misinterprets status |
| **Triggering Conditions** | TC-HF-004 |
| **Scenario Type** | Known Unsafe (A2) - Being mitigated |

**Situation**:
Driver with deuteranopia (red-green color blindness) cannot distinguish between green "active" and red "fault" indicators.

**Potential Hazard**:
- Fault condition interpreted as active/safe
- Driver misses warning state

**Risk Assessment**:
- Severity: S2-S3 (depends on missed warning)
- Probability: Medium (8% of males have color vision deficiency)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Shape differentiation | Different icons for states | Implemented |
| Pattern overlays | Hatching for warning states | Planned |
| High contrast mode | Accessibility setting | Planned |
| Color-blind friendly palette | Design system tokens | Implemented |

**Validation Method**:
- User testing with color vision deficient participants
- Ishihara-style visual tests on UI

---

### 4.6 SOTIF-006: Night Mode Transition

| Attribute | Description |
|-----------|-------------|
| **Scenario ID** | SOTIF-006 |
| **Description** | Sudden mode transition affects dark-adapted vision |
| **Triggering Conditions** | TC-ENV-002 |
| **Scenario Type** | Known Unsafe (A2) - Being mitigated |

**Situation**:
Driving at night with dark-adapted eyes. An alert triggers bright screen flash, temporarily impairing night vision.

**Potential Hazard**:
- Temporary vision impairment
- Difficulty seeing road after bright flash
- Potential collision with unseen obstacle

**Risk Assessment**:
- Severity: S2 (collision with reduced visibility)
- Probability: Medium (night alerts are common)

**Mitigation Strategies**:
| Strategy | Implementation | Status |
|----------|----------------|--------|
| Automatic brightness | Ambient light adaptation | Implemented |
| Night mode palette | Reduced brightness colors | Implemented |
| Alert brightness limit | Max brightness cap at night | Implemented |
| Gradual transitions | No sudden brightness changes | Implemented |

**Validation Method**:
- Dark room testing
- Pupil response measurement

---

## 5. SOTIF Mitigation Summary

### 5.1 Design Mitigations

| Mitigation | Scenarios Addressed | Implementation Component |
|------------|---------------------|-------------------------|
| Large, high-contrast fonts | SOTIF-001, SOTIF-003 | Design system typography |
| Explicit unit display | SOTIF-002 | Speed gauge component |
| Alert priority ladder | SOTIF-003 | AlertManager |
| Visual staleness indicators | SOTIF-004 | ClusterStateModel |
| Shape + color coding | SOTIF-005 | Telltale design |
| Brightness adaptation | SOTIF-006 | Display settings |

### 5.2 Validation Requirements

| SOTIF ID | Validation Method | Pass Criteria |
|----------|-------------------|---------------|
| SOTIF-001 | User testing | 95% correct reading during vibration |
| SOTIF-002 | Comprehension test | 100% unit awareness |
| SOTIF-003 | Cognitive load test | Critical alert response <2s |
| SOTIF-004 | Recognition test | 95% stale recognition |
| SOTIF-005 | Accessibility audit | WCAG 2.1 AA compliance |
| SOTIF-006 | Dark room test | No reported vision impairment |

---

## 6. Unknown Scenario Identification

### 6.1 Analysis Methods

To identify unknown unsafe scenarios (A4):
- Systematic hazard analysis workshops
- Field observation and incident analysis
- User feedback collection
- Competitor analysis
- Research literature review

### 6.2 Open Issues

| Issue ID | Description | Status |
|----------|-------------|--------|
| SOTIF-OPEN-001 | Interaction with AR HUD overlay | Analysis needed |
| SOTIF-OPEN-002 | Multi-screen distraction | Analysis needed |
| SOTIF-OPEN-003 | Autonomous mode transition confusion | Future scope |

---

## 7. SOTIF Checklist

### 7.1 Pre-Release Checklist

- [ ] All known scenarios (A2) have mitigations
- [ ] Mitigations validated per Section 5.2
- [ ] No unacceptable residual risk
- [ ] User documentation addresses known limitations
- [ ] Monitoring plan for post-release scenarios

### 7.2 Post-Release Monitoring

| Activity | Frequency | Owner |
|----------|-----------|-------|
| Incident review | Per incident | Safety team |
| User feedback analysis | Monthly | Product team |
| Field observation | Quarterly | UX research |
| Scenario database update | Per finding | SOTIF engineer |

---

## 8. References

- ISO/PAS 21448:2022 - Road vehicles - Safety of the intended functionality
- SAE J3016 - Taxonomy for driving automation
- NHTSA Human Factors Guidelines
- WCAG 2.1 Accessibility Guidelines

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | SOTIF Engineer | Initial release |
