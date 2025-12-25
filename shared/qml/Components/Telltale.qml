// Telltale.qml
// Warning indicator telltale
// Safety: SR-CL-003 - Critical telltales shall remain visible
// Display-only, state from C++ model

import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import "../DesignSystem"

Item {
    id: root

    // State properties (from C++ model)
    property bool active: false
    property bool valid: true
    property int priority: 2  // 0=critical, 1=warning, 2=info, 3=status

    // Visual properties
    property alias iconSource: icon.source
    property string label: ""
    property bool showLabel: false
    property bool blinking: priority <= 1 && active  // Critical/warning blink

    // Size
    property int iconSize: 32

    width: iconSize + (showLabel ? labelText.implicitWidth + Spacing.sm : 0)
    height: iconSize

    // Telltale icon
    Image {
        id: icon
        width: root.iconSize
        height: root.iconSize
        sourceSize: Qt.size(width, height)
        fillMode: Image.PreserveAspectFit
        opacity: root.active ? 1.0 : Tokens.opacityDisabled
        visible: root.valid

        // Color overlay based on priority
        layer.enabled: true
        layer.effect: ColorOverlay {
            color: root.active ? Colors.telltaleColor(root.priority) : Colors.textTertiary
        }

        // Blinking animation for critical/warning
        SequentialAnimation on opacity {
            id: blinkAnimation
            running: root.blinking
            loops: Animation.Infinite
            NumberAnimation { to: 0.2; duration: Animations.telltaleFlash / 2 }
            NumberAnimation { to: 1.0; duration: Animations.telltaleFlash / 2 }
        }
    }

    // Label
    Text {
        id: labelText
        anchors.left: icon.right
        anchors.leftMargin: Spacing.sm
        anchors.verticalCenter: parent.verticalCenter
        text: root.label
        font.pixelSize: Typography.telltaleLabel
        font.weight: Font.Medium
        color: root.active ? Colors.telltaleColor(root.priority) : Colors.textTertiary
        visible: root.showLabel && root.label !== ""
    }

    // Invalid indicator
    Rectangle {
        anchors.fill: icon
        color: "transparent"
        border.color: Colors.textTertiary
        border.width: 1
        radius: 4
        visible: !root.valid

        Text {
            anchors.centerIn: parent
            text: "?"
            font.pixelSize: root.iconSize * 0.5
            color: Colors.textTertiary
        }
    }
}
