# Turn Indicator Implementation

## Overview

The automotive UI now includes a comprehensive turn indicator system with multiple visual representations for enhanced driver awareness and safety.

## Features

### 1. **TurnIndicator Component** (`TurnIndicator.qml`)
A dedicated QML component for displaying turn signals with:
- **Animated arrow graphics** using Canvas API
- **Blinking animation** (400ms on/off cycle, automotive standard)
- **Glow effect** for enhanced visibility
- **Color-coded states**:
  - Active: `#22C55E` (green)
  - Inactive: `#1A1A1A` (dark gray)
- **Configurable properties**:
  - `active`: Boolean to control indicator state
  - `isLeft`: Boolean to determine arrow direction
  - `activeColor`: Customizable active color
  - `inactiveColor`: Customizable inactive color

### 2. **Multiple Display Locations**

#### Top Telltale Bar (Small)
- **Location**: Center of top telltale bar
- **Size**: 120x80 pixels
- **Purpose**: Standard telltale display
- **Spacing**: 32px between left and right indicators

#### Side Indicators (Large)
- **Location**: Left and right edges of cluster view
- **Size**: 160x120 pixels
- **Purpose**: Enhanced peripheral visibility
- **Behavior**:
  - Fade in/out with opacity animation (200ms)
  - Scale animation (0.8 to 1.0) with OutBack easing
  - Only visible when active

### 3. **Animation Details**

#### Blinking Pattern
```qml
SequentialAnimation {
    NumberAnimation {
        from: 1.0
        to: 0.2
        duration: 400ms
        easing: InOutQuad
    }
    NumberAnimation {
        from: 0.2
        to: 1.0
        duration: 400ms
        easing: InOutQuad
    }
    loops: Infinite
}
```

#### Glow Effect
- Synchronized with blink animation
- Opacity range: 0 to 0.3
- Creates a pulsing halo effect around the arrow

### 4. **Signal Integration**

The turn indicators are connected to the vehicle signal system:

**Signal IDs:**
- Left turn: `telltale.turn_left`
- Right turn: `telltale.turn_right`

**Priority:** 3 (Status indicator)

**Blinking:** Enabled for priority 3 signals (turn indicators)

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Signal Flow                          │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  SignalHub                                              │
│      │                                                  │
│      ├──► TelltaleManager                              │
│      │        │                                         │
│      │        ├──► ClusterViewModel                    │
│      │        │         │                               │
│      │        │         └──► QML (activeTelltales)     │
│      │        │                   │                     │
│      │        │                   ├──► TelltaleBar     │
│      │        │                   │      └─► TurnIndicator (small)
│      │        │                   │                     │
│      │        │                   └──► ClusterView     │
│      │        │                          └─► TurnIndicator (large x2)
│      │        │                                         │
└─────────────────────────────────────────────────────────┘
```

## Files Modified/Created

### Created
- `driver_ui/app/qml/TurnIndicator.qml` - Dedicated turn indicator component

### Modified
- `driver_ui/app/qml/TelltaleBar.qml` - Integrated TurnIndicator components
- `driver_ui/app/qml/ClusterView.qml` - Added large side indicators
- `driver_ui/app/qml/Main.qml` - Added keyboard shortcuts for testing
- `driver_ui/safety_core/cpp/TelltaleManager.cpp` - Updated blinking logic

## Testing

### Keyboard Shortcuts
- **Left Arrow**: Toggle left turn signal
- **Right Arrow**: Toggle right turn signal

### Manual Testing
To test the turn indicators:

1. Build and run the driver UI application
2. Press the Left or Right arrow keys
3. Observe:
   - Small indicators in top telltale bar blink
   - Large side indicators fade in and blink
   - Blinking rate: ~1.25 Hz (800ms cycle)
   - Smooth animations and transitions

### Expected Behavior
- ✅ Indicators blink at automotive-standard rate
- ✅ Glow effect pulses in sync with arrow
- ✅ Side indicators only visible when active
- ✅ Smooth fade-in/out transitions
- ✅ Color changes from inactive to active state
- ✅ Arrow direction matches left/right state

## Safety Considerations

### Compliance
- **SR-CL-003**: Turn signals are part of the telltale system
- **ISO 26262**: Display-only implementation, all logic in C++ safety core
- **Blinking Rate**: Follows automotive standards (~1-2 Hz)

### Fail-Safe Behavior
- If signal becomes invalid, indicator opacity reduces to 0.3
- Turn signals have priority 3 (status), won't interfere with critical warnings
- All animations are GPU-accelerated for consistent performance

## Customization

### Changing Colors
Edit the `activeColor` property in the QML files:

```qml
TurnIndicator {
    activeColor: "#FFB800"  // Amber/orange (traditional)
    // or
    activeColor: "#22C55E"  // Green (current)
}
```

### Adjusting Blink Rate
Modify the animation duration in `TurnIndicator.qml`:

```qml
NumberAnimation {
    duration: 500  // Slower (500ms instead of 400ms)
}
```

### Sizing
Adjust width and height properties:

```qml
// Top bar indicators
TurnIndicator {
    width: 120
    height: 80
}

// Side indicators
TurnIndicator {
    width: 160
    height: 120
}
```

## Future Enhancements

- [ ] Add hazard light mode (both indicators blinking simultaneously)
- [ ] Add sound feedback (optional tick sound)
- [ ] Add auto-cancel after turn completion
- [ ] Add intensity variation based on ambient light
- [ ] Add lane change mode (3 blinks then auto-cancel)

## References

- **ISO 2575**: Road vehicles — Symbols for controls, indicators and tell-tales
- **FMVSS 108**: Lamps, reflective devices, and associated equipment
- **ECE R48**: Installation of lighting and light-signalling devices
