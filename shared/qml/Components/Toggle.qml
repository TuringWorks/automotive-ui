// Toggle.qml
// Toggle switch control

import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import "../DesignSystem"

Item {
    id: root

    property bool checked: false
    property bool enabled: true

    signal toggled(bool checked)

    width: 52
    height: 32
    opacity: enabled ? 1.0 : Tokens.opacityDisabled

    // Track
    Rectangle {
        anchors.fill: parent
        radius: height / 2
        color: root.checked ? Colors.primary : Colors.clusterGaugeTrack

        Behavior on color {
            ColorAnimation { duration: Animations.fast }
        }
    }

    // Thumb
    Rectangle {
        id: thumb
        width: 26
        height: 26
        radius: width / 2
        color: Colors.textPrimary
        anchors.verticalCenter: parent.verticalCenter
        x: root.checked ? parent.width - width - 3 : 3

        Behavior on x {
            NumberAnimation {
                duration: Animations.fast
                easing.type: Easing.OutCubic
            }
        }

        // Shadow
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 1
            radius: 3
            color: "#40000000"
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.enabled
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
    }
}
