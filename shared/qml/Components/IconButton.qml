// IconButton.qml
// Reusable icon button for infotainment

import QtQuick 2.15
import "../DesignSystem"

Rectangle {
    id: root

    property alias iconSource: icon.source
    property string text: ""
    property bool enabled: true
    property bool pressed: mouseArea.pressed
    property bool hovered: mouseArea.containsMouse
    property int iconSize: 24
    property bool showBackground: true

    signal clicked()

    width: row.width + Spacing.lg * 2
    height: Tokens.touchTargetPreferred
    radius: Tokens.radiusSmall
    color: {
        if (!enabled) return "transparent"
        if (pressed) return Colors.withAlpha(Colors.primary, 0.2)
        if (hovered) return Colors.withAlpha(Colors.primary, 0.1)
        return showBackground ? Colors.surface : "transparent"
    }
    opacity: enabled ? 1.0 : Tokens.opacityDisabled

    Row {
        id: row
        anchors.centerIn: parent
        spacing: Spacing.sm

        Image {
            id: icon
            width: root.iconSize
            height: root.iconSize
            sourceSize: Qt.size(width, height)
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: root.text
            font.pixelSize: Typography.sizeMd
            font.weight: Font.Medium
            color: Colors.textPrimary
            anchors.verticalCenter: parent.verticalCenter
            visible: root.text !== ""
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.clicked()
    }
}
