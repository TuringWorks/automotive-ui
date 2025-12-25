// AlertBanner.qml
// Alert/warning banner overlay
// Safety: SR-CL-003 - Cannot be occluded by non-critical overlays
// Priority: P0 (critical) > P1 (warning) > P2 (info) > P3 (status)

import QtQuick 2.15
import "../DesignSystem"

Rectangle {
    id: root

    // Alert properties (from C++ AlertManager)
    property string alertId: ""
    property string title: ""
    property string message: ""
    property int priority: 2  // 0=critical, 1=warning, 2=info, 3=status
    property bool dismissable: priority > 0
    property bool requiresAck: priority <= 1
    property string actionText: ""
    property bool visible: false

    // Signals
    signal dismissed()
    signal acknowledged()
    signal actionTriggered()

    // Computed
    readonly property color alertColor: Colors.telltaleColor(priority)
    readonly property bool isCritical: priority === 0

    // Size
    width: parent ? parent.width : 400
    height: contentColumn.height + Spacing.lg * 2

    // Z-order based on priority (SR-CL-003)
    z: priority === 0 ? Tokens.zCriticalAlert : Tokens.zAlert

    // Appearance
    color: {
        switch(priority) {
            case 0: return Colors.withAlpha(Colors.error, 0.95)
            case 1: return Colors.withAlpha(Colors.warning, 0.9)
            default: return Colors.withAlpha(Colors.surface, 0.95)
        }
    }
    border.color: alertColor
    border.width: priority <= 1 ? 2 : 1
    radius: Tokens.radiusMedium

    // Enter/exit animations
    opacity: visible ? 1 : 0
    scale: visible ? 1 : 0.95

    Behavior on opacity {
        NumberAnimation { duration: Animations.alertEnter; easing.type: Easing.OutCubic }
    }
    Behavior on scale {
        NumberAnimation { duration: Animations.alertEnter; easing.type: Easing.OutBack }
    }

    // Critical alert pulsing border
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: Colors.telltaleRed
        border.width: 3
        radius: parent.radius
        visible: root.isCritical
        opacity: pulseAnimation.running ? pulseOpacity : 1

        property real pulseOpacity: 1

        SequentialAnimation on pulseOpacity {
            id: pulseAnimation
            running: root.isCritical && root.visible
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 500 }
            NumberAnimation { to: 1.0; duration: 500 }
        }
    }

    // Content
    Column {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Spacing.lg
        spacing: Spacing.sm

        Row {
            width: parent.width
            spacing: Spacing.md

            // Priority icon
            Rectangle {
                width: 24
                height: 24
                radius: width / 2
                color: root.alertColor

                Text {
                    anchors.centerIn: parent
                    text: root.priority === 0 ? "!" : (root.priority === 1 ? "⚠" : "ℹ")
                    font.pixelSize: 14
                    font.weight: Font.Bold
                    color: root.priority <= 1 ? Colors.textInverse : Colors.textPrimary
                }
            }

            // Title
            Text {
                width: parent.width - 24 - Spacing.md - (dismissButton.visible ? dismissButton.width + Spacing.md : 0)
                text: root.title
                font.pixelSize: Typography.sizeLg
                font.weight: Font.SemiBold
                color: root.priority <= 1 ? Colors.textInverse : Colors.textPrimary
                elide: Text.ElideRight
            }

            // Dismiss button
            Rectangle {
                id: dismissButton
                width: 32
                height: 32
                radius: width / 2
                color: dismissArea.pressed ? Colors.withAlpha(Colors.textPrimary, 0.2) : "transparent"
                visible: root.dismissable

                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    font.pixelSize: 16
                    color: root.priority <= 1 ? Colors.textInverse : Colors.textPrimary
                }

                MouseArea {
                    id: dismissArea
                    anchors.fill: parent
                    onClicked: root.dismissed()
                }
            }
        }

        // Message
        Text {
            width: parent.width
            text: root.message
            font.pixelSize: Typography.sizeMd
            color: root.priority <= 1 ? Colors.withAlpha(Colors.textInverse, 0.9) : Colors.textSecondary
            wrapMode: Text.WordWrap
            visible: root.message !== ""
        }

        // Action buttons
        Row {
            spacing: Spacing.md
            visible: root.requiresAck || root.actionText !== ""

            // Acknowledge button (for critical/warning)
            Rectangle {
                width: ackText.implicitWidth + Spacing.lg * 2
                height: 40
                radius: Tokens.radiusSmall
                color: ackArea.pressed ? Colors.withAlpha(root.alertColor, 0.8) : root.alertColor
                visible: root.requiresAck

                Text {
                    id: ackText
                    anchors.centerIn: parent
                    text: "Acknowledge"
                    font.pixelSize: Typography.sizeMd
                    font.weight: Font.SemiBold
                    color: Colors.textInverse
                }

                MouseArea {
                    id: ackArea
                    anchors.fill: parent
                    onClicked: root.acknowledged()
                }
            }

            // Custom action button
            Rectangle {
                width: actionTextLabel.implicitWidth + Spacing.lg * 2
                height: 40
                radius: Tokens.radiusSmall
                color: actionArea.pressed ? Colors.withAlpha(Colors.primary, 0.8) : Colors.primary
                visible: root.actionText !== ""

                Text {
                    id: actionTextLabel
                    anchors.centerIn: parent
                    text: root.actionText
                    font.pixelSize: Typography.sizeMd
                    font.weight: Font.SemiBold
                    color: Colors.textInverse
                }

                MouseArea {
                    id: actionArea
                    anchors.fill: parent
                    onClicked: root.actionTriggered()
                }
            }
        }
    }
}
