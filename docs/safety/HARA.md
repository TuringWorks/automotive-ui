# Hazard Analysis and Risk Assessment (HARA)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-HARA-001

---

## 1. Introduction

### 1.1 Purpose

This document presents the Hazard Analysis and Risk Assessment (HARA) for the Driver UI (Cluster) component, following ISO 26262 methodology to identify hazards, assess risks, and determine Automotive Safety Integrity Levels (ASIL).

### 1.2 Scope

Analysis covers all safety-relevant functions of the Driver UI:
- Speed display and indication
- Warning telltale presentation
- Alert notification system
- ADAS status indication
- Signal validity indication

### 1.3 Methodology

ASIL determination uses the standard ISO 26262 risk parameters:
- **Severity (S)**: S0-S3 based on potential harm
- **Exposure (E)**: E0-E4 based on operational frequency
- **Controllability (C)**: C0-C3 based on driver ability to avoid harm

---

## 2. Operational Situations

### 2.1 Situation Catalog

| Situation ID | Description | Speed Range | Traffic | Environment |
|--------------|-------------|-------------|---------|-------------|
| SIT-001 | Highway cruising | 100-130 km/h | Medium | Clear |
| SIT-002 | Highway, adverse weather | 80-120 km/h | Medium | Rain/fog |
| SIT-003 | Urban driving | 30-60 km/h | High | Clear |
| SIT-004 | Urban, night | 30-60 km/h | Medium | Dark |
| SIT-005 | Rural road | 60-100 km/h | Low | Variable |
| SIT-006 | Parking/maneuvering | 0-10 km/h | Variable | Variable |
| SIT-007 | Traffic jam | 0-30 km/h | High | Variable |
| SIT-008 | Emergency braking | Any | Any | Any |

---

## 3. Hazard Identification

### 3.1 Hazard List

| Hazard ID | Hazard Description | Related Function |
|-----------|---------------------|------------------|
| HAZ-001 | Driver misjudges actual vehicle speed | Speed display |
| HAZ-002 | Driver misses critical warning | Telltale display |
| HAZ-003 | Driver believes inactive ADAS is active | ADAS status |
| HAZ-004 | Driver trusts stale/invalid data | Signal validity |
| HAZ-005 | Driver distracted by UI malfunction | Overall display |
| HAZ-006 | Driver misinterprets gear state | Gear indicator |

---

## 4. HARA Table

### 4.1 Complete Analysis

| HARA ID | Hazard | Operational Situation | Malfunctioning Behavior | S | E | C | ASIL | Safety Goal |
|---------|--------|----------------------|-------------------------|---|---|---|------|-------------|
| HARA-001 | HAZ-001 | SIT-001: Highway cruising | Speed displayed lower than actual by >10% | S3 | E4 | C2 | **ASIL C** | SG-001 |
| HARA-002 | HAZ-001 | SIT-003: Urban driving | Speed displayed lower than actual by >10% | S2 | E4 | C2 | **ASIL B** | SG-001 |
| HARA-003 | HAZ-001 | SIT-006: Parking | Speed displayed incorrectly | S1 | E4 | C3 | **ASIL A** | SG-001 |
| HARA-004 | HAZ-002 | SIT-001: Highway | Critical telltale not displayed | S3 | E3 | C2 | **ASIL B** | SG-002 |
| HARA-005 | HAZ-002 | SIT-008: Emergency | Critical telltale not displayed | S3 | E2 | C1 | **ASIL B** | SG-002 |
| HARA-006 | HAZ-003 | SIT-002: Highway adverse | ADAS shown active when inactive | S3 | E3 | C2 | **ASIL B** | SG-003 |
| HARA-007 | HAZ-003 | SIT-003: Urban | ADAS shown active when inactive | S2 | E3 | C2 | **ASIL A** | SG-003 |
| HARA-008 | HAZ-004 | SIT-001: Highway | Stale data displayed without indication | S3 | E3 | C2 | **ASIL B** | SG-004 |
| HARA-009 | HAZ-004 | SIT-003: Urban | Stale data displayed without indication | S2 | E4 | C2 | **ASIL B** | SG-004 |
| HARA-010 | HAZ-005 | SIT-001: Highway | UI frozen/corrupted display | S2 | E2 | C2 | **ASIL A** | SG-005 |
| HARA-011 | HAZ-006 | SIT-006: Parking | Wrong gear displayed | S2 | E4 | C2 | **ASIL B** | SG-006 |

### 4.2 Risk Parameter Rationale

#### Severity Classification
| Rating | Description | Example Outcome |
|--------|-------------|-----------------|
| S0 | No injuries | N/A |
| S1 | Light/moderate injuries | Minor collision at low speed |
| S2 | Severe/life-threatening injuries | Collision with injury |
| S3 | Life-threatening/fatal injuries | High-speed collision |

#### Exposure Classification
| Rating | Description | Probability |
|--------|-------------|-------------|
| E0 | Incredible | <1% of operating time |
| E1 | Very low | 1-2% of operating time |
| E2 | Low | 2-10% of operating time |
| E3 | Medium | 10-50% of operating time |
| E4 | High | >50% of operating time |

#### Controllability Classification
| Rating | Description | Driver Capability |
|--------|-------------|-------------------|
| C0 | Generally controllable | All drivers can avoid |
| C1 | Simply controllable | >99% of drivers can avoid |
| C2 | Normally controllable | >90% of drivers can avoid |
| C3 | Difficult to control | <90% of drivers can avoid |

---

## 5. Safety Goals

### 5.1 Safety Goal Summary

| SG ID | Safety Goal | Derived From | Max ASIL | Safe State |
|-------|-------------|--------------|----------|------------|
| SG-001 | Prevent misleading speed indication | HARA-001,002,003 | ASIL C | Show invalid indicator |
| SG-002 | Ensure critical warnings always visible | HARA-004,005 | ASIL B | Prioritized display |
| SG-003 | Prevent misleading ADAS status | HARA-006,007 | ASIL B | Show inactive/unknown |
| SG-004 | Indicate signal staleness/invalidity | HARA-008,009 | ASIL B | Show stale indicator |
| SG-005 | Prevent distracting display failures | HARA-010 | ASIL A | Degraded mode |
| SG-006 | Ensure correct gear indication | HARA-011 | ASIL B | Show unknown state |

### 5.2 Detailed Safety Goals

#### SG-001: Speed Indication Integrity
**Statement**: The Driver UI shall prevent the display of misleading speed information that could cause the driver to operate at inappropriate speeds.

**Attributes**:
- FTTI (Fault Tolerant Time Interval): 300 ms
- Safe State: Display invalid indicator ("---") and retain last known valid value
- Warning mechanism: Visual invalid/stale indicator

#### SG-002: Warning Visibility
**Statement**: The Driver UI shall ensure that critical warning telltales are always visible and not obscured by other UI elements.

**Attributes**:
- FTTI: 100 ms
- Safe State: Prioritized warning display area always rendered
- Warning mechanism: Dedicated telltale bar with priority management

#### SG-003: ADAS Status Accuracy
**Statement**: The Driver UI shall prevent the indication of active ADAS functions when they are inactive or in an unknown state.

**Attributes**:
- FTTI: 200 ms
- Safe State: Show inactive/unknown status
- Warning mechanism: State-aware icon rendering

#### SG-004: Signal Validity Indication
**Statement**: The Driver UI shall indicate when displayed information is stale, invalid, or of unknown integrity.

**Attributes**:
- FTTI: 300 ms (staleness threshold)
- Safe State: Show validity indicator on affected data
- Warning mechanism: Visual staleness/validity indicators

#### SG-005: Display Integrity
**Statement**: The Driver UI shall prevent display failures that could distract the driver from the driving task.

**Attributes**:
- FTTI: 500 ms
- Safe State: Enter degraded mode with minimal display
- Warning mechanism: Fault detection and degraded mode transition

#### SG-006: Gear Indication Accuracy
**Statement**: The Driver UI shall ensure correct gear state indication or clearly indicate unknown state.

**Attributes**:
- FTTI: 200 ms
- Safe State: Display unknown indicator
- Warning mechanism: Validation of gear state signal

---

## 6. Traceability

### 6.1 HARA to Safety Goals

| HARA ID | Safety Goal(s) |
|---------|---------------|
| HARA-001, HARA-002, HARA-003 | SG-001 |
| HARA-004, HARA-005 | SG-002 |
| HARA-006, HARA-007 | SG-003 |
| HARA-008, HARA-009 | SG-004 |
| HARA-010 | SG-005 |
| HARA-011 | SG-006 |

### 6.2 Safety Goals to Requirements

| Safety Goal | Functional Safety Requirements |
|-------------|-------------------------------|
| SG-001 | FSR-001, FSR-002, SR-CL-001, SR-CL-002 |
| SG-002 | FSR-003, SR-CL-003 |
| SG-003 | FSR-004 |
| SG-004 | FSR-002, SR-CL-001 |
| SG-005 | FSR-005, SR-CL-004 |
| SG-006 | FSR-006 |

---

## 7. Review and Approval

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Safety Engineer | | | |
| Independent Reviewer | | | |
| Safety Manager | | | |

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
