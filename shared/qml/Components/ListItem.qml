// ListItem.qml
// Standard list item for lists and menus

import QtQuick 2.15
import "../DesignSystem"

Rectangle {
    id: root

    property string title: ""
    property string subtitle: ""
    property alias leadingContent: leadingLoader.sourceComponent
    property alias trailingContent: trailingLoader.sourceComponent
    property bool showDivider: true
    property bool interactive: true
    property bool hovered: mouseArea.containsMouse
    property bool pressed: mouseArea.pressed

    signal clicked()

    height: Math.max(Tokens.touchTargetPreferred, contentRow.height + Spacing.md * 2)
    color: {
        if (pressed) return Colors.withAlpha(Colors.primary, 0.1)
        if (hovered) return Colors.withAlpha(Colors.primary, 0.05)
        return "transparent"
    }

    Row {
        id: contentRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: Spacing.lg
        spacing: Spacing.lg

        // Leading content (icon, avatar, etc.)
        Loader {
            id: leadingLoader
            anchors.verticalCenter: parent.verticalCenter
        }

        // Text content
        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - leadingLoader.width - trailingLoader.width - Spacing.lg * 2
            spacing: Spacing.xxs

            Text {
                width: parent.width
                text: root.title
                font.pixelSize: Typography.sizeLg
                font.weight: Font.Medium
                color: Colors.textPrimary
                elide: Text.ElideRight
            }

            Text {
                width: parent.width
                text: root.subtitle
                font.pixelSize: Typography.sizeSm
                color: Colors.textSecondary
                elide: Text.ElideRight
                visible: root.subtitle !== ""
            }
        }

        // Trailing content (chevron, switch, etc.)
        Loader {
            id: trailingLoader
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // Bottom divider
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: leadingLoader.width > 0 ? leadingLoader.width + Spacing.lg * 2 : Spacing.lg
        height: 1
        color: Colors.border
        visible: root.showDivider
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.interactive
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.clicked()
    }
}
