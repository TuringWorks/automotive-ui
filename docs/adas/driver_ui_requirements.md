# Driver UI ADAS Requirements

## Overview

This document specifies the ADAS-related requirements for the Driver UI, including engagement status display, takeover request handling, and perception visualization.

## Component Overview

### AdasStateZone

Primary engagement status display area located in the center console region.

**Layout:**
```
┌────────────────────────────────────────────────────────┐
│                    ADAS State Zone                      │
│  ┌─────────────────────────────────────────────────┐   │
│  │ [Level Badge]  ENGAGED - L2 PARTIAL AUTOMATION  │   │
│  │ ─────────────────────────────────────────────── │   │
│  │ Driver in control                                │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌───────────┐  ┌───────────┐  ┌───────────────────┐   │
│  │ [ACC ✓]   │  │ [LKA ✓]   │  │ Set: 75 mph      │   │
│  │           │  │           │  │ Gap: 2.0 sec     │   │
│  └───────────┘  └───────────┘  └───────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ [Hands-On Reminder - when applicable]           │   │
│  └─────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────┘
```

**Requirements:**

| ID | Requirement | Verification |
|----|-------------|--------------|
| DRV-ADAS-001 | Display current automation level prominently | Visual inspection |
| DRV-ADAS-002 | Show responsibility owner text | Unit test |
| DRV-ADAS-003 | Display active features as badges | Visual inspection |
| DRV-ADAS-004 | Show set speed and following gap | Unit test |
| DRV-ADAS-005 | Display hands-on reminder when required | Unit test |
| DRV-ADAS-006 | Update within 100ms of state change | Timing test |

### TakeoverBanner

Full-width banner for takeover requests with escalation states.

**Layout:**
```
┌─────────────────────────────────────────────────────────────────────┐
│ ⚠ TAKE OVER                                          ┌─────┐       │
│ ODD ending - lane lines not detected                 │ 10  │ sec   │
│                                                      └─────┘       │
└─────────────────────────────────────────────────────────────────────┘
```

**Requirements:**

| ID | Requirement | Verification |
|----|-------------|--------------|
| DRV-TOR-001 | z-index 1000 - cannot be occluded | Visual inspection |
| DRV-TOR-002 | Display countdown timer | Unit test |
| DRV-TOR-003 | Show reason for takeover | Unit test |
| DRV-TOR-004 | Escalate visual urgency on progression | Animation test |
| DRV-TOR-005 | Display "IMMEDIATE ACTION REQUIRED" at threshold | Unit test |
| DRV-TOR-006 | MRM state shows "SAFE STOP IN PROGRESS" | Unit test |

**Urgency Color Mapping:**

| Urgency Level | Background | Text | Animation |
|---------------|------------|------|-----------|
| Advisory | Amber 20% | White | None |
| Warning | Amber 80% | White | Pulse 1Hz |
| Immediate | Orange 100% | White | Flash 2Hz |
| Critical | Red 100% | White | Flash 3Hz |
| MRM | Red 100% | White | Solid glow |

### PerceptionMiniView

Compact 2D visualization of perception data.

**Layout:**
```
┌───────────────────────────┐
│        Perception         │
│  ┌─────────────────────┐  │
│  │     ▼ (lead)        │  │
│  │    ─┼─ (ego)        │  │
│  │  │       │  lanes   │  │
│  │  │       │          │  │
│  └─────────────────────┘  │
│  Lane Quality: ████░ 80%  │
└───────────────────────────┘
```

**Requirements:**

| ID | Requirement | Verification |
|----|-------------|--------------|
| DRV-PCP-001 | Show lane boundaries with confidence colors | Visual inspection |
| DRV-PCP-002 | Display ego vehicle position | Visual inspection |
| DRV-PCP-003 | Show lead vehicle when detected | Unit test |
| DRV-PCP-004 | Display lane quality indicator | Unit test |
| DRV-PCP-005 | Update at minimum 10 Hz | Performance test |
| DRV-PCP-006 | Hide stale data within 200ms | Timing test |

## State Transitions

### Engagement Flow

```
User Activates ADAS
        │
        ▼
┌──────────────────┐
│ Check Available  │
└────────┬─────────┘
         │
    ┌────┴────┐
    ▼         ▼
Available  Unavailable
    │         │
    ▼         ▼
 Engage    Show reason
    │
    ▼
Display engaged state
within 100ms
```

### Disengagement Flow

```
Disengagement Trigger
        │
        ▼
┌──────────────────────┐
│   Capture reason     │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│ Transition to OFF    │
│ within 100ms         │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│ Log event to HMI log │
└──────────────────────┘
```

## Timing Requirements

| Requirement | Maximum Latency | Verification Method |
|-------------|-----------------|---------------------|
| State change display | 100ms | Signal timing test |
| Unavailable indication | 300ms | State transition test |
| Degraded indication | 300ms | Perception staleness test |
| Takeover banner display | 100ms | UI responsiveness test |
| Stale marker hiding | 200ms | Freshness validation test |

## Safety Compliance

### SR-CL-ADAS-100: Engagement Validity

```cpp
bool isValidEngagement(const AdasEngagement& engagement) {
    return engagement.metadata.valid &&
           engagement.engaged &&
           engagement.available;
}
```

The UI MUST NOT display "ENGAGED" unless all three conditions are true.

### SR-CL-ADAS-120: Takeover Non-Occlusion

The TakeoverBanner MUST:
- Have z-index of 1000 (highest in the UI layer)
- Span full width of the display
- Not be dismissable by user action during active request
- Remain visible until acknowledged or cancelled by system

### SR-CL-ADAS-121: Urgency HMI Patterns

| Urgency | Visual Pattern | Audio Pattern |
|---------|----------------|---------------|
| Advisory | Steady amber | Single chime |
| Warning | Pulsing amber | Repeated chimes |
| Immediate | Flashing orange | Continuous tone |
| Critical | Flashing red | Alarm tone |

## Integration Points

### Signal Connections

```cpp
// Connect ADAS state service to view model
connect(stateService, &AdasStateService::stateChanged,
        viewModel, &AdasViewModel::onStateChanged);

connect(stateService, &AdasStateService::engagedChanged,
        viewModel, &AdasViewModel::onEngagedChanged);

// Connect takeover manager
connect(takeoverManager, &TakeoverManager::takeoverRequested,
        viewModel, &AdasViewModel::onTakeoverRequested);

connect(takeoverManager, &TakeoverManager::takeoverEscalated,
        viewModel, &AdasViewModel::onTakeoverEscalated);
```

### QML Property Bindings

```qml
AdasStateZone {
    hmiState: adasViewModel.hmiState
    automationLevel: adasViewModel.automationLevel
    engaged: adasViewModel.engaged
    available: adasViewModel.available
    features: adasViewModel.activeFeatures
    setSpeed: adasViewModel.setSpeed
    followingGap: adasViewModel.followingGap
}

TakeoverBanner {
    visible: adasViewModel.takeoverActive
    urgency: adasViewModel.takeoverUrgency
    countdown: adasViewModel.takeoverCountdown
    message: adasViewModel.takeoverMessage
}
```

## Test Coverage

| Component | Unit Tests | Integration Tests | UI Tests |
|-----------|------------|-------------------|----------|
| AdasViewModel | test_adas_view_model.cpp | test_signal_flow.cpp | - |
| AdasStateZone | - | - | test_adas_state_zone.qml |
| TakeoverBanner | - | - | test_takeover_banner.qml |
| PerceptionMiniView | - | - | test_perception_mini.qml |

## Related Documentation

- [ADAS UI Overview](overview.md)
- [Automation Levels Matrix](levels_matrix.md)
- [Usage Scenarios](scenarios.md)
