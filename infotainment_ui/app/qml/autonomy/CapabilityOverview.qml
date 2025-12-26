// CapabilityOverview.qml
// ADAS Capability Overview Display
// Part of: Infotainment Autonomy Center
// Spec: FR-INF-ADAS-001

import QtQuick
import QtQuick.Layouts
import DesignSystem 1.0

/**
 * Capability Overview - Shows installed and allowed automation features.
 *
 * Displays:
 * - Current automation level
 * - Available features (ACC, LKA, LCA, etc.)
 * - Feature status (active, available, unavailable)
 * - User responsibility based on level
 */
Rectangle {
    id: root

    property int automationLevel: 0
    property bool engaged: false

    // Feature configuration (would come from vehicle config)
    property var features: [
        { id: "acc", name: "Adaptive Cruise Control", available: true, active: false, minLevel: 1 },
        { id: "lka", name: "Lane Keeping Assist", available: true, active: false, minLevel: 1 },
        { id: "lca", name: "Lane Centering", available: true, active: false, minLevel: 2 },
        { id: "bsm", name: "Blind Spot Monitoring", available: true, active: true, minLevel: 0 },
        { id: "aeb", name: "Auto Emergency Braking", available: true, active: true, minLevel: 0 },
        { id: "tja", name: "Traffic Jam Assist", available: false, active: false, minLevel: 2 },
        { id: "hda", name: "Highway Driving Assist", available: true, active: false, minLevel: 3 }
    ]

    radius: 12
    color: Colors.surfaceSecondary
    implicitHeight: contentColumn.height + Spacing.large * 2

    ColumnLayout {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Spacing.large
        spacing: Spacing.medium

        // Header
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "System Capabilities"
                font: Typography.headingMedium
                color: Colors.textPrimary
            }

            Item { Layout.fillWidth: true }

            // Level badge
            Rectangle {
                width: 48
                height: 32
                radius: 4
                color: getLevelColor()

                Text {
                    anchors.centerIn: parent
                    text: "L" + root.automationLevel
                    font: Typography.labelMedium
                    color: Colors.textOnPrimary
                }
            }
        }

        // Level description
        Text {
            Layout.fillWidth: true
            text: getLevelDescription()
            font: Typography.bodyMedium
            color: Colors.textSecondary
            wrapMode: Text.WordWrap
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Colors.border
        }

        // Feature grid
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: Spacing.small
            columnSpacing: Spacing.medium

            Repeater {
                model: root.features

                FeatureItem {
                    Layout.fillWidth: true
                    name: modelData.name
                    available: modelData.available
                    active: modelData.active
                    meetsLevel: root.automationLevel >= modelData.minLevel
                }
            }
        }

        // SAE Level explanation link
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Spacing.small

            Text {
                text: "Learn more about SAE automation levels"
                font: Typography.caption
                color: Colors.primary

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        // Open education modal
                        console.log("Open SAE levels education")
                    }
                }
            }

            Item { Layout.fillWidth: true }
        }
    }

    // Helper functions
    function getLevelColor() {
        switch (root.automationLevel) {
            case 0: return Colors.surfaceTertiary
            case 1: return Colors.info
            case 2: return Colors.info
            case 3: return Colors.warning
            case 4:
            case 5: return Colors.success
            default: return Colors.surfaceTertiary
        }
    }

    function getLevelDescription() {
        switch (root.automationLevel) {
            case 0: return "No driving automation. Driver performs all driving tasks."
            case 1: return "Driver Assistance. System provides steering OR speed support. Driver is always responsible."
            case 2: return "Partial Automation. System provides steering AND speed support. Driver must continuously supervise."
            case 3: return "Conditional Automation. System drives within ODD. Driver must be ready to take over when requested."
            case 4: return "High Automation. System drives and handles fallback within ODD."
            case 5: return "Full Automation. System can drive in all conditions."
            default: return ""
        }
    }

    // Feature item component
    component FeatureItem: Rectangle {
        property string name: ""
        property bool available: false
        property bool active: false
        property bool meetsLevel: false

        height: 48
        radius: 8
        color: active ? Colors.successBackground :
               (available && meetsLevel ? Colors.surfaceTertiary : "#00000000")
        border.color: active ? Colors.success :
                      (available && meetsLevel ? Colors.border : Colors.border)
        border.width: 1
        opacity: available ? 1.0 : 0.5

        RowLayout {
            anchors.fill: parent
            anchors.margins: Spacing.small
            spacing: Spacing.small

            // Status indicator
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: active ? Colors.success :
                       (available && meetsLevel ? Colors.info : Colors.textTertiary)
            }

            // Feature name
            Text {
                Layout.fillWidth: true
                text: name
                font: Typography.bodySmall
                color: available ? Colors.textPrimary : Colors.textTertiary
                elide: Text.ElideRight
            }

            // Status text
            Text {
                text: active ? "Active" :
                      (available && meetsLevel ? "Ready" :
                       (available ? "L" + (meetsLevel ? "" : "+") : "N/A"))
                font: Typography.caption
                color: active ? Colors.success :
                       (available ? Colors.textSecondary : Colors.textTertiary)
            }
        }
    }
}
