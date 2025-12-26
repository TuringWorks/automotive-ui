# SAE J3016 Automation Levels Matrix

## Overview

This document maps SAE J3016 automation levels to HMI states, driver responsibilities, and UI elements as implemented in the ADAS UI system.

## Automation Level Definitions

| Level | Name | DDT Execution | DDT Fallback | ODD Scope |
|-------|------|---------------|--------------|-----------|
| L0 | No Automation | Driver | Driver | N/A |
| L1 | Driver Assistance | Driver + System | Driver | Limited |
| L2 | Partial Automation | System | Driver | Limited |
| L3 | Conditional Automation | System | System → Driver | Limited |
| L4 | High Automation | System | System | Defined |
| L5 | Full Automation | System | System | Unlimited |

DDT = Dynamic Driving Task

## HMI State Mapping

| Automation Level | Primary HMI State | Engagement Display | Responsibility Owner |
|------------------|-------------------|-------------------|---------------------|
| L0 | Off | Not shown | Driver |
| L1 | EngagedL1 | "Assistive" | Driver |
| L2 | EngagedL2 | "Hands On" | Driver |
| L3 | EngagedL3 | "Eyes Off" | System (within ODD) |
| L4+ | EngagedL3* | "Fully Autonomous" | System |

*L4/L5 use EngagedL3 state with different display text

## Responsibility Indicators

### Driver Responsibility (L0-L2)

- Primary color: Blue (#2196F3)
- Message: "Driver in control" / "Keep hands on wheel"
- Visual emphasis on driver monitoring requirements

### Shared Responsibility (L2 Transition)

- Primary color: Amber (#FFC107)
- Message: "System assisting - stay alert"
- Visual cues for attention requirements

### System Responsibility (L3+)

- Primary color: Green (#4CAF50)
- Message: "System in control"
- Clear indication of when driver can disengage

## Feature Availability by Level

| Feature | L0 | L1 | L2 | L3 | L4+ |
|---------|----|----|----|----|-----|
| Forward Collision Warning | Y | Y | Y | Y | Y |
| Lane Departure Warning | Y | Y | Y | Y | Y |
| Adaptive Cruise Control | - | Y | Y | Y | Y |
| Lane Keep Assist | - | Y | Y | Y | Y |
| Lane Change Assist | - | - | Y | Y | Y |
| Traffic Jam Assist | - | - | Y | Y | Y |
| Highway Pilot | - | - | - | Y | Y |
| Urban Pilot | - | - | - | - | Y |

## Takeover Request Handling

| Level | TOR Required | TOR Timing | Fallback on No Response |
|-------|--------------|------------|------------------------|
| L0-L2 | N/A | N/A | N/A |
| L3 | Yes | 10+ seconds | MRM (Minimal Risk Maneuver) |
| L4 | Conditional | 10+ seconds | Autonomous safe stop |
| L5 | No | N/A | Autonomous resolution |

## ODD (Operational Design Domain) Constraints

### Displayed ODD Factors

| Factor | L1-L2 | L3 | L4+ |
|--------|-------|----|----|
| Speed Range | Optional | Required | Required |
| Road Type | Optional | Required | Required |
| Weather | Warning only | Required | Required |
| Time of Day | Not shown | Optional | Required |
| Geofence | Not shown | Required | Required |

### ODD Exit Handling

```
ODD Exit Detected
       │
       ▼
┌──────────────────┐
│  Urgency Level?  │
└────────┬─────────┘
         │
    ┌────┼────┬────┐
    ▼    ▼    ▼    ▼
Advisory Warning Immediate Critical
   │      │      │       │
   ▼      ▼      ▼       ▼
30+ sec  10 sec  5 sec  3 sec
Pre-warn Active Escalate MRM
```

## DMS (Driver Monitoring) Requirements

| Level | Hands-On | Eyes-On | Attention Monitoring |
|-------|----------|---------|---------------------|
| L0 | Not monitored | Not monitored | Not monitored |
| L1 | Recommended | Not monitored | Not monitored |
| L2 | Required | Required | Active |
| L3 | Not required | Not required | Active (for TOR) |

### DMS State Indicators

| State | Icon | Message | Action |
|-------|------|---------|--------|
| HandsOn | Steering wheel | None | None |
| HandsOff | Warning wheel | "Place hands on wheel" | Audible alert |
| AttentionGood | None | None | None |
| AttentionWarning | Eye icon | "Stay alert" | Visual alert |
| AttentionCritical | Eye + ⚠ | "Look at road!" | Audible + haptic |

## Visual Treatment by Level

### Color Coding

| Level | Primary | Accent | Alert |
|-------|---------|--------|-------|
| L0 | Grey | - | Red |
| L1 | Blue | Light blue | Amber |
| L2 | Blue | Teal | Amber |
| L3 | Green | Light green | Red |
| L4+ | Green | Cyan | Red |

### Animation Patterns

| Level | Engagement | Disengagement | Alert |
|-------|------------|---------------|-------|
| L1 | Fade in 300ms | Fade out 200ms | Pulse 1Hz |
| L2 | Slide + fade 400ms | Slide out 200ms | Pulse 2Hz |
| L3 | Expand + fade 500ms | Collapse 300ms | Flash 3Hz |

## Implementation References

### AutomationLevel Enum

```cpp
enum class AutomationLevel {
    L0_NoAutomation,
    L1_DriverAssistance,
    L2_PartialAutomation,
    L3_ConditionalAutomation,
    L4_HighAutomation,
    L5_FullAutomation
};
```

### Responsibility Determination

```cpp
QString responsibilityOwner(AutomationLevel level) {
    switch (level) {
        case AutomationLevel::L0_NoAutomation:
        case AutomationLevel::L1_DriverAssistance:
        case AutomationLevel::L2_PartialAutomation:
            return "Driver";
        case AutomationLevel::L3_ConditionalAutomation:
        case AutomationLevel::L4_HighAutomation:
        case AutomationLevel::L5_FullAutomation:
            return "System";
    }
}
```

## Related Documentation

- [ADAS UI Overview](overview.md)
- [Driver UI Requirements](driver_ui_requirements.md)
- [Usage Scenarios](scenarios.md)
