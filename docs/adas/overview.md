# ADAS UI Architecture Overview

## Purpose

This document provides an architectural overview of the ADAS (Advanced Driver Assistance Systems) UI implementation, covering both the Driver UI integration and the Infotainment Autonomy Center.

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              Vehicle ECU Layer                               │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐                 │
│  │ ADAS Controller │  │ Perception ECU │  │   DMS ECU      │                 │
│  └───────┬────────┘  └───────┬────────┘  └───────┬────────┘                 │
└──────────┼───────────────────┼───────────────────┼──────────────────────────┘
           │                   │                   │
           └───────────────────┼───────────────────┘
                               │ IPC (Validated, Signed)
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          ADAS Core Services Layer                            │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │                         AdasStateService                                 ││
│  │  • State machine (10 HMI states)                                        ││
│  │  • Anti-replay protection (sequence numbers)                            ││
│  │  • Freshness validation (300ms window)                                  ││
│  └─────────────────────────────────────────────────────────────────────────┘│
│  ┌───────────────────┐ ┌────────────────────┐ ┌──────────────────────────┐ │
│  │  PerceptionModel   │ │  TakeoverManager   │ │ AdasVisualQualityManager │ │
│  │  • Lane detection  │ │  • Escalation logic│ │ • Frame rate adaptation  │ │
│  │  • Lead vehicle    │ │  • Countdown timer │ │ • Thermal management     │ │
│  │  • Object tracking │ │  • MRM transition  │ │ • Quality levels         │ │
│  └───────────────────┘ └────────────────────┘ └──────────────────────────┘ │
│  ┌───────────────────────────────────────────────────────────────────────┐  │
│  │                          HmiEventLog                                   │  │
│  │  • Tamper-evident logging with chained checksums                      │  │
│  │  • Event categories: Engagement, Degradation, Takeover, Security      │  │
│  └───────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
                               │
           ┌───────────────────┴───────────────────┐
           ▼                                       ▼
┌─────────────────────────────────┐   ┌─────────────────────────────────────┐
│        Driver UI                 │   │     Infotainment UI                  │
│  ┌─────────────────────────────┐│   │  ┌─────────────────────────────────┐│
│  │      AdasViewModel          ││   │  │     AutonomyCenterPage          ││
│  │  (C++ to QML bridge)        ││   │  │  • CapabilityOverview           ││
│  └──────────────┬──────────────┘│   │  │  • WhyUnavailable               ││
│                 │               │   │  │  • EventTimeline                ││
│  ┌──────────────▼──────────────┐│   │  │  • OddViewer                    ││
│  │      AdasStateZone          ││   │  │  • PerceptionExplorer           ││
│  │  • Engagement status        ││   │  │  • AutonomySettings             ││
│  │  • Feature badges           ││   │  └─────────────────────────────────┘│
│  │  • Speed/gap display        ││   │                                      │
│  └─────────────────────────────┘│   │  Motion-locked engineering mode     │
│  ┌─────────────────────────────┐│   └─────────────────────────────────────┘
│  │      TakeoverBanner         ││
│  │  • z-index: 1000            ││
│  │  • Escalation animations    ││
│  └─────────────────────────────┘│
│  ┌─────────────────────────────┐│
│  │    PerceptionMiniView       ││
│  │  • Lane visualization       ││
│  │  • Lead vehicle indicator   ││
│  └─────────────────────────────┘│
└─────────────────────────────────┘
                │
                ▼
┌─────────────────────────────────┐
│    Qt Quick 3D Visualization     │
│  ┌─────────────────────────────┐│
│  │        AdasScene3D          ││
│  │  • Environment presets      ││
│  │  • Quality scaling          ││
│  └─────────────────────────────┘│
│  ┌───────────┐ ┌───────────────┐│
│  │RoadVisual │ │LaneBoundaryLine││
│  └───────────┘ └───────────────┘│
│  ┌───────────┐ ┌───────────────┐│
│  │LeadVehicle│ │ObjectInstances ││
│  └───────────┘ └───────────────┘│
│  ┌─────────────────────────────┐│
│  │     WeatherParticles        ││
│  │  Rain, Snow, Fog            ││
│  └─────────────────────────────┘│
└─────────────────────────────────┘
```

## Core Components

### AdasStateService

The central state machine managing ADAS HMI states:

| State | Description | Entry Condition |
|-------|-------------|-----------------|
| Off | System inactive | Default / system off |
| Available | Ready for engagement | available=true, engaged=false |
| EngagedL1 | Assistive features active | L1 automation engaged |
| EngagedL2 | Partial automation | L2 automation engaged |
| EngagedL3 | Conditional automation | L3 automation engaged |
| TakeoverRequested | Driver takeover needed | ODD exit / system request |
| MrmActive | Minimal Risk Maneuver | Takeover timeout / critical fault |
| Degraded | Reduced capability | Sensor/system degradation |
| FaultLockout | System unavailable | Critical fault detected |

### TakeoverManager

Handles driver handoff with escalating urgency:

| Urgency | Initial State | Countdown | Escalation |
|---------|---------------|-----------|------------|
| Advisory | PreWarning | 30+ sec | None |
| Warning | RequestActive | 10 sec | +5 sec |
| Immediate | Escalated | 5 sec | +3 sec |
| Critical | CriticalEscalation | 3 sec | MRM |

### PerceptionModel

Manages perception data with freshness validation:

- Lane boundaries with confidence levels
- Lead vehicle detection and tracking
- Object list with classification
- Drivable corridor visualization

### AdasVisualQualityManager

Adaptive quality management for visualization:

| Level | Resolution | MSAA | Reflections | Particles |
|-------|------------|------|-------------|-----------|
| Ultra | 100% | 8x | SSR | Full |
| High | 100% | 4x | SSR | High |
| Medium | 85% | 2x | None | Medium |
| Low | 70% | None | None | Low |
| Minimal | 50% | None | None | None |

## Safety Requirements Traceability

| Requirement ID | Description | Implementation |
|----------------|-------------|----------------|
| SR-CL-ADAS-100 | Never show ENGAGED unless valid | AdasStateService::isEngaged() |
| SR-CL-ADAS-101 | Unavailable within 300ms | State transition timing |
| SR-CL-ADAS-102 | Default to OFF on unknown | Initial state = Off |
| SR-CL-ADAS-120 | Non-occludable takeover | TakeoverBanner z-index 1000 |
| SR-CL-ADAS-121 | Urgency HMI mapping | Color/animation patterns |
| SR-CL-ADAS-122 | Immediate action threshold | 3-second countdown trigger |
| SR-CL-ADAS-130 | MRM persistent state | MrmActive state display |
| SR-CL-ADAS-140 | Degraded within 300ms | Perception freshness check |
| SR-CL-ADAS-141 | Lane quality reflection | Confidence-based styling |
| SR-CL-ADAS-142 | Hide stale markers | 200ms freshness window |

## Cybersecurity Requirements Traceability

| Requirement ID | Description | Implementation |
|----------------|-------------|----------------|
| CR-ADAS-102 | Anti-replay protection | Sequence number validation |
| CR-ADAS-103 | Default to OFF on integrity failure | State machine fallback |
| CR-INF-ADAS-010 | Tamper-evident logging | HmiEventLog chained checksums |
| CR-INF-ADAS-111 | Secure event log | Event category/severity tracking |
| CR-INF-ADAS-120 | Motion-locked engineering | PerceptionExplorer motion check |

## File Structure

```
shared/
├── cpp/
│   └── adas/
│       ├── AdasTypes.h              # Core enumerations and structs
│       ├── AdasEngagement.h         # Engagement and ODD data types
│       ├── AdasStateService.h/cpp   # Central state machine
│       ├── PerceptionModel.h/cpp    # Perception data management
│       ├── TakeoverManager.h/cpp    # Takeover escalation logic
│       ├── HmiEventLog.h/cpp        # Tamper-evident logging
│       └── AdasVisualQualityManager.h/cpp  # Quality adaptation
└── qml/
    └── Adas3D/
        ├── AdasScene3D.qml          # Main 3D scene
        ├── RoadVisualization.qml    # Road surface and markings
        ├── LaneBoundaryLine.qml     # Lane confidence visualization
        ├── LeadVehicleMarker.qml    # Lead vehicle representation
        ├── ObjectInstances.qml      # Instanced object rendering
        └── WeatherParticles.qml     # Weather particle effects

driver_ui/
├── app/
│   ├── cpp/
│   │   └── AdasViewModel.h/cpp      # C++ to QML bridge
│   └── qml/
│       └── adas/
│           ├── AdasStateZone.qml    # Engagement status display
│           ├── TakeoverBanner.qml   # Takeover request banner
│           └── PerceptionMiniView.qml  # Mini lane visualization

infotainment_ui/
└── app/
    └── qml/
        └── autonomy/
            ├── AutonomyCenterPage.qml   # Main autonomy page
            ├── CapabilityOverview.qml   # Feature availability
            ├── WhyUnavailable.qml       # Limitation explanations
            ├── EventTimeline.qml        # Event history
            ├── OddViewer.qml            # ODD status display
            ├── PerceptionExplorer.qml   # Engineering mode
            └── AutonomySettings.qml     # User preferences
```

## Related Documentation

- [Automation Levels Matrix](levels_matrix.md)
- [Driver UI Requirements](driver_ui_requirements.md)
- [Infotainment Requirements](infotainment_requirements.md)
- [Usage Scenarios](scenarios.md)
- [Security Architecture](security.md)
