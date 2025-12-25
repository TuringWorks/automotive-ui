# Design System Quick Reference

## Using DesignTokens in Your QML Files

### Import the Design System
```qml
import QtQuick 2.15
// DesignTokens is available as a singleton - no import needed if in same module
```

### Colors

#### Background Colors
```qml
Rectangle {
    color: "#03090F"           // colorBackground
    color: "#0B1118"           // colorBackgroundElevated
    color: "#111821"           // colorBackgroundCard
    color: "#1A2229"           // colorBackgroundHover
    color: "#1F3442"           // colorBackgroundActive
}
```

#### Surface Colors (for Media/Content)
```qml
Rectangle {
    color: "#0F0F0F"           // colorSurface
    color: "#1A1A1A"           // colorSurfaceElevated
    color: "#262626"           // colorSurfaceHover
}
```

#### Text Colors
```qml
Text {
    color: "#F4F7FB"           // colorTextPrimary (main content)
    color: "#8AA3B8"           // colorTextSecondary (supporting)
    color: "#7F95A6"           // colorTextTertiary (labels)
    color: "#6F8797"           // colorTextMuted (hints)
}
```

#### Accent Colors
```qml
Rectangle {
    color: "#4DC9CB"           // colorAccentPrimary (cyan)
    color: "#60A5FA"           // colorAccentBlue
    color: "#22C55E"           // colorAccentGreen
    color: "#A78BFA"           // colorAccentPurple
    color: "#F472B6"           // colorAccentPink
    color: "#F59E0B"           // colorAccentOrange
}
```

### Spacing

```qml
Item {
    anchors.margins: 4         // spacing4
    anchors.margins: 8         // spacing8
    anchors.margins: 12        // spacing12
    anchors.margins: 16        // spacing16
    anchors.margins: 20        // spacing20
    anchors.margins: 24        // spacing24
    anchors.margins: 32        // spacing32
    anchors.margins: 48        // spacing48
}
```

### Border Radius

```qml
Rectangle {
    radius: 8                  // radiusSmall
    radius: 12                 // radiusMedium
    radius: 16                 // radiusLarge
    radius: 20                 // radiusXLarge
    radius: 24                 // radiusXXLarge
    radius: 32                 // radiusRound
}
```

### Typography

```qml
Text {
    font.pixelSize: 11         // fontSizeXSmall
    font.pixelSize: 12         // fontSizeSmall
    font.pixelSize: 14         // fontSizeMedium
    font.pixelSize: 16         // fontSizeBase
    font.pixelSize: 18         // fontSizeLarge
    font.pixelSize: 20         // fontSizeXLarge
    font.pixelSize: 24         // fontSizeXXLarge
    font.pixelSize: 34         // fontSizeHuge
}
```

### Shadows

```qml
import Qt5Compat.GraphicalEffects

Rectangle {
    layer.enabled: true
    layer.effect: DropShadow {
        color: "#00000040"     // shadowSmall
        color: "#00000060"     // shadowMedium
        color: "#00000080"     // shadowLarge
        color: "#00000090"     // shadowXLarge
    }
}
```

### Animation Durations

```qml
Behavior on property {
    NumberAnimation {
        duration: 150          // durationFast
        duration: 250          // durationNormal
        duration: 350          // durationSlow
    }
}

// Or for transitions
Behavior on property {
    NumberAnimation {
        duration: 200          // transitionQuick
        duration: 300          // transitionSmooth
    }
}
```

## Common Patterns

### Interactive Button with Hover

```qml
Rectangle {
    id: button
    width: 120
    height: 44
    radius: 22
    color: mouseArea.containsMouse ? "#1F2830" : "#111821"
    
    // Smooth color transition
    Behavior on color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    // Scale on hover
    scale: mouseArea.containsMouse ? 1.05 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    Text {
        anchors.centerIn: parent
        text: "Button"
        font.pixelSize: 14
        color: "#F4F7FB"
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onClicked: {
            // Click feedback
            button.scale = 0.95
            clickTimer.start()
            // Your action here
        }
    }
    
    Timer {
        id: clickTimer
        interval: 100
        onTriggered: button.scale = 1.0
    }
}
```

### Card with Shadow

```qml
Rectangle {
    width: 200
    height: 150
    radius: 20
    color: "#111821"
    border.width: 1
    border.color: "#1C2832"
    
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: 4
        radius: 12
        samples: 17
        color: "#00000060"
    }
}
```

### Hover Card with Animation

```qml
Rectangle {
    id: card
    width: 200
    height: 150
    radius: 20
    color: mouseArea.containsMouse ? "#1F2830" : "#111821"
    border.width: 1
    border.color: mouseArea.containsMouse ? "#2A3844" : "#1C2832"
    
    Behavior on color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    Behavior on border.color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    scale: mouseArea.containsMouse ? 1.02 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: mouseArea.containsMouse ? 8 : 4
        radius: mouseArea.containsMouse ? 16 : 12
        samples: 17
        color: "#00000060"
        
        Behavior on verticalOffset {
            NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
        }
        
        Behavior on radius {
            NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }
}
```

### Progress Bar with Glow

```qml
Rectangle {
    width: 300
    height: 5
    radius: 2.5
    color: "#14222D"
    
    Rectangle {
        width: parent.width * 0.6  // 60% progress
        height: parent.height
        radius: parent.radius
        color: "#4DC9CB"
        
        Behavior on width {
            NumberAnimation { duration: 100; easing.type: Easing.Linear }
        }
        
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8
            samples: 17
            color: "#4DC9CB40"
        }
    }
}
```

### Icon with Glow

```qml
Rectangle {
    width: 56
    height: 56
    radius: 16
    color: "#60A5FA"
    
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: 0
        radius: 12
        samples: 17
        color: "#60A5FA50"
    }
    
    Text {
        anchors.centerIn: parent
        text: "🎵"
        font.pixelSize: 28
    }
}
```

### Text Hierarchy

```qml
Column {
    spacing: 8
    
    // Main heading
    Text {
        text: "Main Title"
        font.pixelSize: 24
        font.weight: Font.Bold
        color: "#F4F7FB"
        font.letterSpacing: -0.5
    }
    
    // Subheading
    Text {
        text: "Subtitle"
        font.pixelSize: 16
        font.weight: Font.DemiBold
        color: "#8AA3B8"
        font.letterSpacing: 0.2
    }
    
    // Body text
    Text {
        text: "Description text"
        font.pixelSize: 14
        color: "#7F95A6"
        font.letterSpacing: 0.3
    }
    
    // Helper text
    Text {
        text: "Helper or hint text"
        font.pixelSize: 12
        color: "#6F8797"
    }
}
```

## Best Practices

### DO ✅
- Use design system colors consistently
- Apply smooth transitions (200-250ms)
- Use OutCubic easing for natural feel
- Maintain minimum 44px touch targets
- Add hover states to all interactive elements
- Provide click feedback animations
- Use proper visual hierarchy
- Apply shadows for depth
- Use letter spacing for readability

### DON'T ❌
- Don't use arbitrary colors
- Don't skip animations on state changes
- Don't use instant transitions
- Don't make touch targets too small
- Don't forget hover states
- Don't skip visual feedback
- Don't use too many font sizes
- Don't overuse shadows
- Don't ignore spacing system

## Animation Guidelines

### Timing
- **Quick feedback**: 100ms (click response)
- **Standard**: 200ms (hover, colors)
- **Smooth**: 250ms (complex changes)
- **Slow**: 350ms (page transitions)

### Easing
- **OutCubic**: Most transitions (smooth deceleration)
- **Linear**: Progress bars, continuous animations
- **InOutCubic**: Two-way animations

### Scale Values
- **Hover**: 1.02 - 1.1x (depending on element)
- **Click**: 0.9 - 0.96x (press feedback)
- **Normal**: 1.0x

## Accessibility

### Touch Targets
- Minimum: 44x44px
- Recommended: 48x48px or larger
- Spacing: At least 8px between targets

### Contrast
- Primary text: High contrast (#F4F7FB on dark)
- Secondary text: Medium contrast (#8AA3B8)
- Disabled: Low contrast (#6F8797)

### Visual Feedback
- Always show hover state
- Always show active/selected state
- Always provide click feedback
- Always change cursor on interactive elements

## Performance Tips

1. **Use layer.enabled sparingly** - Only when needed for effects
2. **Optimize animations** - Keep duration short and smooth
3. **Minimize repaints** - Use Behavior for smooth transitions
4. **Efficient shadows** - Use appropriate sample counts
5. **GPU acceleration** - Let Qt handle transform animations

## Example Component

Here's a complete example combining all best practices:

```qml
Rectangle {
    id: premiumButton
    width: 140
    height: 48
    radius: 24
    color: mouseArea.containsMouse ? "#1F2830" : "#111821"
    border.width: 1
    border.color: mouseArea.containsMouse ? "#2A3844" : "#1C2832"
    
    // Smooth transitions
    Behavior on color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    Behavior on border.color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    // Hover scale
    scale: mouseArea.containsMouse ? 1.03 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
    
    // Shadow
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: mouseArea.containsMouse ? 6 : 3
        radius: mouseArea.containsMouse ? 14 : 10
        samples: 17
        color: "#00000060"
        
        Behavior on verticalOffset {
            NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
        }
        
        Behavior on radius {
            NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
        }
    }
    
    Row {
        anchors.centerIn: parent
        spacing: 8
        
        Text {
            text: "🎵"
            font.pixelSize: 18
            anchors.verticalCenter: parent.verticalCenter
        }
        
        Text {
            text: "Premium Button"
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: mouseArea.containsMouse ? "#FFFFFF" : "#E8F3FB"
            font.letterSpacing: 0.2
            anchors.verticalCenter: parent.verticalCenter
            
            Behavior on color {
                ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
            }
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onClicked: {
            premiumButton.scale = 0.96
            clickTimer.start()
            console.log("Button clicked!")
        }
    }
    
    Timer {
        id: clickTimer
        interval: 100
        onTriggered: premiumButton.scale = 1.0
    }
}
```

This creates a beautiful, interactive button with:
- ✅ Smooth hover effects
- ✅ Click feedback
- ✅ Dynamic shadows
- ✅ Proper colors from design system
- ✅ Accessible touch target (48px height)
- ✅ Visual hierarchy
- ✅ Premium feel
