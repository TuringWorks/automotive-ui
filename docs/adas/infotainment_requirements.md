# Infotainment Autonomy Center Requirements

## Overview

The Autonomy Center is a comprehensive infotainment application providing transparency into ADAS/AD system capabilities, limitations, and historical behavior. It is classified as QM (non-safety) but cybersecurity-critical.

## Application Structure

```
AutonomyCenterPage
├── Overview Tab (CapabilityOverview, WhyUnavailable)
├── ODD Map Tab (OddViewer)
├── Events Tab (EventTimeline)
├── Settings Tab (AutonomySettings)
└── Engineering Tab (PerceptionExplorer) [Motion-locked]
```

## Component Requirements

### AutonomyCenterPage

Main container with tab-based navigation.

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-AC-001 | Tab navigation between sections | UI test |
| INF-AC-002 | Motion lock overlay for engineering mode | Integration test |
| INF-AC-003 | Preserve tab state on navigation | UI test |
| INF-AC-004 | Display current system status in header | Unit test |

### CapabilityOverview

Grid display of all ADAS features with availability status.

**Layout:**
```
┌─────────────────────────────────────────────────────────┐
│ Feature Capabilities                                     │
│ ┌─────────────────────────────────────────────────────┐ │
│ │  ACC     LKA     LCA     TJA     HWP               │ │
│ │  [✓]     [✓]     [!]     [-]     [✓]               │ │
│ │ Active  Active  Limited  Off   Available           │ │
│ └─────────────────────────────────────────────────────┘ │
│                                                          │
│ Current Automation Level                                 │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ [●] Level 2 - Partial Automation                    │ │
│ │     Driver remains responsible                       │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-CO-001 | Display all ADAS features in grid | UI test |
| INF-CO-002 | Show feature status (Active/Available/Limited/Off) | Unit test |
| INF-CO-003 | Display current automation level | Unit test |
| INF-CO-004 | Explain responsibility at current level | Unit test |
| INF-CO-005 | Link to detailed feature info | UI test |

### WhyUnavailable

Explains why features are currently unavailable (SOTIF-UI-001).

**Layout:**
```
┌─────────────────────────────────────────────────────────┐
│ Why is Lane Change Assist unavailable?                   │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ ⚠ Speed below minimum (40 mph required)            │ │
│ │ ⚠ Lane markings not clearly detected               │ │
│ │ ✓ Camera system operational                        │ │
│ │ ✓ Radar system operational                         │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-WU-001 | List all current limitations | Unit test |
| INF-WU-002 | Icon for each limitation type | Visual inspection |
| INF-WU-003 | Description of each limitation | Unit test |
| INF-WU-004 | Show met conditions as satisfied | Unit test |
| INF-WU-005 | Update when conditions change | Integration test |

### OddViewer

Displays Operational Design Domain boundaries and status.

**Layout:**
```
┌─────────────────────────────────────────────────────────┐
│ Operational Design Domain                                │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ [Map placeholder showing ODD boundary]              │ │
│ │                                                      │ │
│ │ Current position: Inside ODD                        │ │
│ └─────────────────────────────────────────────────────┘ │
│                                                          │
│ ODD Constraints                                          │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Speed: 0-85 mph ✓                                   │ │
│ │ Road Type: Highway only ✓                           │ │
│ │ Weather: Clear to light rain ✓                      │ │
│ │ Lighting: Day or night ✓                            │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-OD-001 | Display current ODD status | Unit test |
| INF-OD-002 | Show ODD boundaries on map | Integration test |
| INF-OD-003 | List all ODD constraints | Unit test |
| INF-OD-004 | Indicate constraint satisfaction | Unit test |
| INF-OD-005 | Warn when approaching ODD boundary | Integration test |

### EventTimeline

Historical record of ADAS events.

**Layout:**
```
┌─────────────────────────────────────────────────────────┐
│ Event History                    [Filter ▼] [Today ▼]   │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ 14:32 [INFO] ACC Engaged at 65 mph                  │ │
│ │ 14:28 [WARN] Lane departure warning                 │ │
│ │ 14:15 [INFO] System became available                │ │
│ │ 13:45 [WARN] Perception degraded - rain             │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-ET-001 | Display events in reverse chronological order | Unit test |
| INF-ET-002 | Filter by event category | UI test |
| INF-ET-003 | Filter by severity | UI test |
| INF-ET-004 | Severity-based color coding | Visual inspection |
| INF-ET-005 | Timestamp format HH:mm | Unit test |
| INF-ET-006 | Event count summary | Unit test |

### AutonomySettings

User preferences for ADAS features.

**Sections:**
- Feature Toggles (ACC, LKA, LCA, etc.)
- Alert Preferences (audio, visual, haptic)
- DMS Settings (sensitivity levels)
- Calibration Status

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-AS-001 | Toggle switches for features | UI test |
| INF-AS-002 | Persist settings across restarts | Integration test |
| INF-AS-003 | Immediate effect on toggle | Integration test |
| INF-AS-004 | Alert type selection | UI test |
| INF-AS-005 | DMS sensitivity levels | Unit test |
| INF-AS-006 | Show calibration status | Unit test |

### PerceptionExplorer (Engineering Mode)

Advanced perception visualization, motion-locked per CR-INF-ADAS-120.

**Features:**
- Environment preset selection (Dawn/Day/Dusk/Night)
- Weather particle toggles (Rain/Snow/Fog)
- Perception data overlay
- Event simulation controls

| ID | Requirement | Verification |
|----|-------------|--------------|
| INF-PE-001 | Motion lock when vehicle moving | Integration test |
| INF-PE-002 | Speed threshold < 5 mph for access | Unit test |
| INF-PE-003 | Environment preset selection | UI test |
| INF-PE-004 | Weather particle toggles | UI test |
| INF-PE-005 | Real-time perception data display | Integration test |
| INF-PE-006 | Event simulator for testing | UI test |

**Motion Lock Overlay:**
```
┌─────────────────────────────────────────────────────────┐
│                                                          │
│              ⚠ Vehicle in Motion                        │
│                                                          │
│    Engineering mode requires vehicle to be stopped       │
│    (speed < 5 mph)                                       │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## Cybersecurity Requirements

### CR-INF-ADAS-010: Tamper-Evident Logging

All ADAS events must be logged using the HmiEventLog with chained checksums:

```cpp
void logEvent(EventCategory category, EventSeverity severity,
              const QString& message, const QVariantMap& data) {
    HmiEvent event;
    event.timestamp = QDateTime::currentMSecsSinceEpoch();
    event.category = category;
    event.severity = severity;
    event.message = message;
    event.data = data;
    event.checksum = computeChainedChecksum(event, lastChecksum);

    eventLog.append(event);
    lastChecksum = event.checksum;
}
```

### CR-INF-ADAS-111: Event Categories

| Category | Description | Examples |
|----------|-------------|----------|
| Engagement | System activation/deactivation | ACC engaged, LKA disengaged |
| Degradation | Capability reduction | Perception degraded, sensor fault |
| Takeover | Driver handoff events | TOR issued, TOR acknowledged |
| Security | Security-related events | Replay rejected, integrity failure |
| User | User actions | Setting changed, feature toggled |

### CR-INF-ADAS-120: Motion Lock

Engineering mode access requires:
1. Vehicle speed < 5 mph
2. Parking brake engaged (recommended)
3. User acknowledgment

## Visual Quality Adaptation

The Autonomy Center adapts visual quality based on system load:

| Quality Level | 3D Scene | Particles | Animations |
|---------------|----------|-----------|------------|
| Ultra | Full | Full | All |
| High | Full | Reduced | All |
| Medium | Simplified | Minimal | Core only |
| Low | Basic | None | Essential |
| Minimal | Disabled | None | None |

## Integration Points

### Data Sources

```cpp
// Connect to ADAS services
connect(adasStateService, &AdasStateService::stateChanged,
        autonomyModel, &AutonomyModel::onStateChanged);

connect(perceptionModel, &PerceptionModel::laneModelChanged,
        autonomyModel, &AutonomyModel::onLaneModelChanged);

connect(hmiEventLog, &HmiEventLog::eventLogged,
        autonomyModel, &AutonomyModel::onEventLogged);
```

### Settings Storage

```cpp
// Secure settings for user preferences
SecureSettings settings;
settings.setValue("adas/acc_enabled", true);
settings.setValue("adas/lka_enabled", true);
settings.setValue("adas/alert_audio", true);
settings.setValue("adas/dms_sensitivity", "medium");
```

## Test Coverage

| Component | Unit Tests | Integration Tests | UI Tests |
|-----------|------------|-------------------|----------|
| CapabilityOverview | capability_test.cpp | - | test_capability.qml |
| WhyUnavailable | limitations_test.cpp | - | test_unavailable.qml |
| OddViewer | odd_test.cpp | test_odd_map.cpp | test_odd_viewer.qml |
| EventTimeline | timeline_test.cpp | test_event_log.cpp | test_timeline.qml |
| AutonomySettings | settings_test.cpp | test_persistence.cpp | test_settings.qml |
| PerceptionExplorer | - | test_motion_lock.cpp | test_explorer.qml |

## Related Documentation

- [ADAS UI Overview](overview.md)
- [Security Architecture](security.md)
- [Usage Scenarios](scenarios.md)
