// Card.qml
// Card container for content

import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import "../DesignSystem"

Rectangle {
    id: root

    property bool elevated: true
    property bool interactive: false
    property bool hovered: interactiveArea.containsMouse
    property bool pressed: interactiveArea.pressed

    signal clicked()

    color: Colors.surfaceElevated
    radius: Tokens.radiusMedium
    border.color: Colors.border
    border.width: 1

    // Elevation effect
    layer.enabled: elevated
    layer.effect: DropShadow {
        horizontalOffset: Tokens.shadowMedium.horizontalOffset
        verticalOffset: Tokens.shadowMedium.verticalOffset
        radius: Tokens.shadowMedium.radius
        color: Tokens.shadowMedium.color
    }

    // Hover/press states
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: {
            if (pressed) return Colors.withAlpha(Colors.primary, 0.1)
            if (hovered) return Colors.withAlpha(Colors.primary, 0.05)
            return "transparent"
        }
        visible: root.interactive
    }

    MouseArea {
        id: interactiveArea
        anchors.fill: parent
        enabled: root.interactive
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.clicked()
    }
}
