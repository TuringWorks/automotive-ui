# Item Definition (SAFE-ITEM-001)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-ITEM-001

---

## 1. Item Description

### 1.1 Overview

The **Driver UI (Cluster)** is a software component that renders safety-relevant vehicle information to the driver through a graphical display. It provides:

- **Speed indication** with validity and staleness status
- **Gear/transmission state** display
- **Warning telltales** for critical vehicle conditions
- **Prioritized alerts** for driver notification
- **Degraded mode** operation when signal integrity is compromised

### 1.2 Safety Relevance

The Driver UI influences driver decisions by presenting information about:
- Current vehicle speed (affects speed choice)
- Warning conditions (affects immediate response)
- ADAS status (affects driver engagement)
- System health (affects trust and response)

Incorrect or misleading information may result in:
- Inappropriate speed for conditions
- Missed warnings requiring immediate action
- Over-reliance on inactive safety systems
- Delayed response to critical conditions

### 1.3 Item Boundary

```
┌─────────────────────────────────────────────────────────────────┐
│                        Driver UI Item                           │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    Safety Core (C++)                     │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌──────────────────┐    │   │
│  │  │ClusterState │ │ AlertMgr    │ │ DegradedMode     │    │   │
│  │  │   Model     │ │             │ │   Controller     │    │   │
│  │  └─────────────┘ └─────────────┘ └──────────────────┘    │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌──────────────────┐    │   │
│  │  │ Telltale    │ │ Safety      │ │ Fault            │    │   │
│  │  │   Manager   │ │   Monitor   │ │   Injector       │    │   │
│  │  └─────────────┘ └─────────────┘ └──────────────────┘    │   │
│  └──────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                  QML Display Layer                       │   │
│  │  (Display-only, no safety logic)                         │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
        │                                              ▲
        │ Signal requests                              │ Validated signals
        ▼                                              │
┌─────────────────────────────────────────────────────────────────┐
│                    Shared Signal Layer                          │
│  ┌─────────────┐ ┌─────────────┐ ┌──────────────────────────┐   │
│  │ SignalHub   │ │ Signal      │ │ Vehicle Signals          │   │
│  │             │ │  Validator  │ │                          │   │
│  └─────────────┘ └─────────────┘ └──────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
        ▲
        │ Raw signals (from ECU/Simulation)
        │
   [External Interface - Out of Scope]
```

---

## 2. Operational Scenarios

### 2.1 Normal Operation Scenarios

| Scenario ID | Description | Conditions | Expected Behavior |
|-------------|-------------|------------|-------------------|
| OP-001 | City driving | Speed 0-50 mph, frequent stops | Full cluster display, responsive updates |
| OP-002 | Highway driving | Speed 50-85 mph, sustained | Full cluster display, stable readings |
| OP-003 | Parking | Speed 0-5 mph, maneuvering | Full cluster with gear prominence |
| OP-004 | Startup sequence | Ignition on | Self-test, then normal display |
| OP-005 | Shutdown sequence | Ignition off | Graceful state preservation |

### 2.2 Environmental Scenarios

| Scenario ID | Description | Conditions | UI Considerations |
|-------------|-------------|------------|-------------------|
| ENV-001 | Night driving | Low ambient light | Dark theme, reduced brightness available |
| ENV-002 | Direct sunlight | High ambient light | High contrast requirements |
| ENV-003 | Rain/fog | Reduced visibility | Critical info prominence |
| ENV-004 | Cold start | -40°C to -20°C | Display warm-up time considered |
| ENV-005 | Hot environment | +50°C to +85°C | Thermal management active |

### 2.3 Fault Scenarios

| Scenario ID | Description | Trigger | Expected Behavior |
|-------------|-------------|---------|-------------------|
| FAULT-001 | Single signal stale | No update >300ms | Stale indicator, value retained |
| FAULT-002 | Multiple signals stale | Bus overload | Partial degraded mode |
| FAULT-003 | Signal out of range | Sensor fault | Invalid indicator, clamped display |
| FAULT-004 | Complete signal loss | CAN bus failure | Full degraded mode |
| FAULT-005 | Render stall | GPU/driver fault | Watchdog intervention |

---

## 3. Interfaces

### 3.1 Input Interfaces

| Interface ID | Source | Data | Rate | Protocol |
|--------------|--------|------|------|----------|
| IF-IN-001 | VehicleSignalsService | Speed | ≥10 Hz | IPC (validated) |
| IF-IN-002 | VehicleSignalsService | RPM | ≥10 Hz | IPC (validated) |
| IF-IN-003 | VehicleSignalsService | Gear state | Event-driven | IPC (validated) |
| IF-IN-004 | VehicleSignalsService | Fuel level | 1 Hz | IPC (validated) |
| IF-IN-005 | VehicleSignalsService | Coolant temp | 1 Hz | IPC (validated) |
| IF-IN-006 | VehicleSignalsService | Telltale states | Event-driven | IPC (validated) |
| IF-IN-007 | AlertService | Alert requests | Event-driven | IPC (prioritized) |

### 3.2 Output Interfaces

| Interface ID | Destination | Data | Type |
|--------------|-------------|------|------|
| IF-OUT-001 | Display | Rendered cluster | Visual |
| IF-OUT-002 | Audio system | Alert sounds (stub) | Audio |
| IF-OUT-003 | Logging service | State transitions | Diagnostic |

### 3.3 External Dependencies

| Dependency | Requirement | Assumption |
|------------|-------------|------------|
| Operating System | POSIX-compatible | Deterministic scheduling available |
| GPU Driver | OpenGL ES 3.0+ | Stable frame delivery |
| Display Hardware | ≥60 Hz refresh | Automotive qualified |
| Qt Framework | 6.5+ | Validated for automotive |

---

## 4. Functional Requirements

### 4.1 Speed Display (SR-CL-001)

**Requirement**: The speed display shall update at ≥10 Hz with staleness detection within 300ms.

| Attribute | Value |
|-----------|-------|
| Update rate | ≥10 Hz nominal |
| Staleness threshold | 300 ms |
| Display range | 0-300 km/h (0-186 mph) |
| Resolution | 1 km/h (1 mph) |
| Validity indication | Required |

### 4.2 Invalid Signal Handling (SR-CL-002)

**Requirement**: Invalid or out-of-range signals shall be clamped to safe display values and indicated as invalid.

| Signal | Valid Range | Clamp Behavior | Invalid Display |
|--------|-------------|----------------|-----------------|
| Speed | 0-300 km/h | Clamp to range | "---" or last valid |
| RPM | 0-10000 | Clamp to range | "---" |
| Fuel | 0-100% | Clamp to range | Empty indicator |
| Coolant | -40 to 150°C | Clamp to range | Warning state |

### 4.3 Critical Telltale Visibility (SR-CL-003)

**Requirement**: Critical warning telltales shall always be visible and not obscured by other UI elements.

| Telltale Category | Priority | Visibility Requirement |
|-------------------|----------|------------------------|
| Red warnings | Highest | Never obscured |
| Amber warnings | High | May be queued, never hidden |
| Informational | Normal | Standard visibility |

### 4.4 Degraded Mode (SR-CL-004)

**Requirement**: On loss of multiple critical signals, the UI shall enter degraded mode showing minimal essential information.

| Degraded Level | Trigger | Display Content |
|----------------|---------|-----------------|
| Normal | All signals valid | Full cluster |
| Partial | 1-2 signals stale | Full cluster with indicators |
| Degraded | 3+ signals stale | Essential info + warning |
| Minimal | Complete signal loss | Fault message only |

---

## 5. Non-Functional Requirements

### 5.1 Timing Requirements

| Requirement | Value | Rationale |
|-------------|-------|-----------|
| Signal processing latency | ≤50 ms | Driver perception threshold |
| Display update latency | ≤16.7 ms (60 Hz) | Smooth visual update |
| Alert display latency | ≤100 ms | Rapid warning response |
| Degraded mode entry | ≤500 ms | Timely fault indication |

### 5.2 Reliability Requirements

| Requirement | Target | Measurement |
|-------------|--------|-------------|
| MTBF (software) | >10,000 hours | Field data collection |
| Availability | >99.9% | Uptime monitoring |
| Startup time | <3 seconds | Boot measurement |

---

## 6. Safety Partitioning

### 6.1 Architecture Principle

**RULE**: No safety-critical logic shall execute in QML/JavaScript.

All safety-relevant processing occurs in the C++ Safety Core:
- Signal validation
- Staleness detection
- Range checking
- Alert prioritization
- Degraded mode decisions
- State machine transitions

QML is strictly for display rendering based on validated C++ properties.

### 6.2 Memory Partitioning

| Component | Allocation Strategy | Rationale |
|-----------|---------------------|-----------|
| Safety Core | Static/bounded | Deterministic behavior |
| Signal buffers | Pre-allocated pools | No runtime allocation |
| Alert queue | Fixed-size ring buffer | Bounded memory |
| QML layer | Qt managed | Non-safety |

---

## 7. References

- SAFE-PLAN-001: Safety Plan
- HARA Document: Hazard Analysis
- FSC Document: Functional Safety Concept
- TSC Document: Technical Safety Concept

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
