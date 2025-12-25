# Functional Safety Concept (FSC)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-FSC-001

---

## 1. Introduction

### 1.1 Purpose

This document defines the Functional Safety Concept (FSC) for the Driver UI component, mapping Safety Goals to Functional Safety Requirements and describing the architectural safety mechanisms.

### 1.2 Scope

Covers functional safety mechanisms for:
- Signal validation and integrity
- Warning and alert management
- Degraded mode operation
- Display integrity monitoring

---

## 2. Safety Goal to FSR Mapping

### 2.1 SG-001: Speed Indication Integrity → FSR-001, FSR-002

| FSR ID | Requirement | ASIL | Allocation |
|--------|-------------|------|------------|
| FSR-001 | Speed signal shall be validated for range (0-300 km/h), rate-of-change (≤15 m/s²), and freshness (≤300ms) | ASIL C | SignalValidator, ClusterStateModel |
| FSR-002 | Invalid speed shall trigger visual indication within 100ms and display last valid value or "---" | ASIL C | ClusterStateModel, ClusterView |

**Functional Safety Mechanism**:
```
┌─────────────────────────────────────────────────────────────┐
│                   Speed Validation Chain                    │
├─────────────────────────────────────────────────────────────┤
│  Raw Speed ──► Range Check ──► Rate Check ──► Freshness     │
│      │              │              │              │         │
│      ▼              ▼              ▼              ▼         │
│  [0-300]      [Δv/Δt≤15]    [age≤300ms]    [Valid/Stale]    │
│                                                             │
│  Any failure ──► Set Invalid ──► Show Indicator             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 SG-002: Warning Visibility → FSR-003

| FSR ID | Requirement | ASIL | Allocation |
|--------|-------------|------|------------|
| FSR-003 | Critical telltales shall be rendered in dedicated area with guaranteed visibility, never obscured by other elements | ASIL B | TelltaleManager, AlertManager, TelltaleBar |

**Functional Safety Mechanism**:
```
┌─────────────────────────────────────────────────────────────┐
│               Telltale Priority Management                  │
├─────────────────────────────────────────────────────────────┤
│  Priority Levels:                                           │
│    P0 (Critical) - Always visible, dedicated space          │
│    P1 (Warning)  - High priority, queued if needed          │
│    P2 (Info)     - Normal priority, may be deferred         │
│                                                             │
│  Display Rules:                                             │
│    - Critical telltales have reserved screen area           │
│    - Alerts cannot overlay critical telltale bar            │
│    - Z-order enforced: Telltales > Alerts > Content         │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 SG-003: ADAS Status Accuracy → FSR-004

| FSR ID | Requirement | ASIL | Allocation |
|--------|-------------|------|------------|
| FSR-004 | ADAS status indicators shall show "inactive" or "unknown" state when status signal is invalid, stale, or indicates inactive | ASIL B | ClusterStateModel |

**Functional Safety Mechanism**:
- Default state: Inactive/Unknown
- State change requires valid, fresh signal indicating active
- Signal loss → Immediate transition to Unknown
- No "stuck active" failure mode possible

### 2.4 SG-004: Signal Validity Indication → FSR-002

Covered by FSR-002 (validity indication applies to all safety-relevant signals).

### 2.5 SG-005: Display Integrity → FSR-005

| FSR ID | Requirement | ASIL | Allocation |
|--------|-------------|------|------------|
| FSR-005 | Display health shall be monitored; frame render failures exceeding threshold shall trigger degraded mode | ASIL A | SafetyMonitor, DegradedModeController |

**Functional Safety Mechanism**:
```
┌─────────────────────────────────────────────────────────────┐
│               Display Health Monitoring                     │
├─────────────────────────────────────────────────────────────┤
│  Frame Heartbeat:                                           │
│    - Expected: 60 FPS (16.7ms interval)                     │
│    - Tolerance: Up to 3 missed frames                       │
│    - Threshold: >5 consecutive misses → Warning             │
│    - Action: >10 consecutive misses → Degraded mode         │
│                                                             │
│  Watchdog Timer:                                            │
│    - Period: 100ms                                          │
│    - Action: Reset render pipeline on expiry                │
└─────────────────────────────────────────────────────────────┘
```

### 2.6 SG-006: Gear Indication Accuracy → FSR-006

| FSR ID | Requirement | ASIL | Allocation |
|--------|-------------|------|------------|
| FSR-006 | Gear state shall be validated; unknown or invalid gear signal shall display "?" indicator | ASIL B | ClusterStateModel, GearIndicator |

---

## 3. Functional Safety Architecture

### 3.1 Safety Element Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    Functional Safety Elements                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐       │
│  │   Signal     │    │   State      │    │   Degraded   │       │
│  │  Validation  │───►│   Model      │───►│    Mode      │       │
│  │   (FSR-001)  │    │   (FSR-002)  │    │   (FSR-005)  │       │
│  └──────────────┘    └──────────────┘    └──────────────┘       │
│         │                   │                   │               │
│         │                   ▼                   │               │
│         │            ┌──────────────┐           │               │
│         │            │   Alert &    │           │               │
│         └───────────►│  Telltale    │◄──────────┘               │
│                      │  Management  │                           │
│                      │   (FSR-003)  │                           │
│                      └──────────────┘                           │
│                             │                                   │
│                             ▼                                   │
│                      ┌──────────────┐                           │
│                      │   Display    │                           │
│                      │   Output     │                           │
│                      │   (QML)      │                           │
│                      └──────────────┘                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 Safety Mechanism Details

#### 3.2.1 Signal Validation Mechanism

**Implementation**: `SignalValidator`, `SignalHub`

| Check Type | Criteria | Action on Failure |
|------------|----------|-------------------|
| Range | Within defined min/max | Mark OutOfRange, clamp value |
| Rate | Δvalue/Δtime ≤ threshold | Mark Invalid, retain previous |
| Freshness | age ≤ staleness threshold | Mark Stale, retain value |
| Plausibility | Cross-signal consistency | Mark Invalid if inconsistent |

#### 3.2.2 Alert Priority Mechanism

**Implementation**: `AlertManager`

| Priority | Category | Behavior |
|----------|----------|----------|
| 0 | Safety Critical | Immediate display, cannot be dismissed |
| 1 | Warning | Display within 100ms, requires acknowledgment |
| 2 | Caution | Queued display, auto-dismiss option |
| 3 | Information | Queued display, dismissible |

#### 3.2.3 Degraded Mode State Machine

**Implementation**: `DegradedModeController`

```
                    ┌─────────┐
                    │  Normal │
                    └────┬────┘
                         │ 1-2 signals stale
                         ▼
                    ┌─────────┐
                    │ Partial │
                    └────┬────┘
                         │ 3+ signals stale
                         ▼
                    ┌──────────┐
                    │ Degraded │
                    └────┬─────┘
                         │ Complete signal loss
                         ▼
                    ┌─────────┐
                    │ Minimal │
                    └─────────┘
```

| State | Display Content | Entry Condition | Exit Condition |
|-------|-----------------|-----------------|----------------|
| Normal | Full cluster | All critical signals valid | N/A |
| Partial | Full + stale indicators | 1-2 critical signals stale | All signals valid for 5s |
| Degraded | Essential only + warning | 3+ critical signals stale | <3 signals stale for 5s |
| Minimal | Fault message only | Complete signal loss | Any signal recovery |

---

## 4. Safe States

### 4.1 Safe State Definitions

| Safe State ID | Description | Visual State | Entry Trigger |
|---------------|-------------|--------------|---------------|
| SS-001 | Speed Invalid | Display "---", stale icon | Speed validation failure |
| SS-002 | Warning Mode | Dedicated warning display | Critical telltale active |
| SS-003 | Degraded Display | Minimal info + fault banner | Multiple signal loss |
| SS-004 | Blank Safe | Controlled blank with fault LED | Complete render failure |

### 4.2 Fault Tolerant Time Intervals (FTTI)

| Function | FTTI | Rationale |
|----------|------|-----------|
| Speed indication | 300 ms | Driver perception + reaction |
| Telltale update | 100 ms | Immediate warning response |
| ADAS status | 200 ms | Driver awareness interval |
| Degraded mode entry | 500 ms | Graceful transition |

---

## 5. Allocation to Components

### 5.1 Software Component Allocation

| Component | FSRs | Safety Contribution |
|-----------|------|---------------------|
| SignalHub | FSR-001 | Signal distribution and routing |
| SignalValidator | FSR-001 | Range, rate, freshness validation |
| ClusterStateModel | FSR-001, FSR-002, FSR-004, FSR-006 | State management, validity tracking |
| AlertManager | FSR-003 | Alert prioritization and lifecycle |
| TelltaleManager | FSR-003 | Telltale state management |
| DegradedModeController | FSR-005 | Degraded mode state machine |
| SafetyMonitor | FSR-005 | Health monitoring, watchdog |

### 5.2 Hardware Assumptions

| Assumption | Requirement | Verification |
|------------|-------------|--------------|
| Display refresh | ≥60 Hz | Hardware specification review |
| GPU stability | No undefined behavior | Driver qualification |
| Memory integrity | ECC or parity (if safety-critical HW) | Hardware design review |

---

## 6. Traceability

### 6.1 Safety Goals to FSRs

| Safety Goal | FSR(s) |
|-------------|--------|
| SG-001 | FSR-001, FSR-002 |
| SG-002 | FSR-003 |
| SG-003 | FSR-004 |
| SG-004 | FSR-002 |
| SG-005 | FSR-005 |
| SG-006 | FSR-006 |

### 6.2 FSRs to Technical Safety Requirements

| FSR | TSR(s) |
|-----|--------|
| FSR-001 | TSR-001, TSR-002, TSR-003 |
| FSR-002 | TSR-004, TSR-005 |
| FSR-003 | TSR-006, TSR-007 |
| FSR-004 | TSR-008 |
| FSR-005 | TSR-009, TSR-010 |
| FSR-006 | TSR-011 |

---

## 7. References

- SAFE-HARA-001: Hazard Analysis and Risk Assessment
- SAFE-TSC-001: Technical Safety Concept
- SAFE-ITEM-001: Item Definition

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
