// AdasStateZone.qml
// ADAS State Display Zone for Driver UI Cluster
// Part of: Driver UI Application Layer
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.1

import QtQuick 2.15
import QtQuick.Layouts 1.15

/**
 * ADAS State Zone - Primary display of ADAS engagement status.
 *
 * Shows:
 * - Engagement state (Available/Engaged/Off)
 * - Active features (ACC, LKA, LCA)
 * - Set speed and following gap
 * - Responsibility indicator
 *
 * Safety Requirements:
 * - FR-CL-ADAS-001: Display availability vs engagement distinctly
 * - SR-CL-ADAS-100: Never show ENGAGED unless valid
 */
Rectangle {
    id: root

    // Data bindings (from AdasViewModel)
    property int hmiState: 0
    property string hmiStateText: "Off"
    property int automationLevel: 0
    property bool available: false
    property bool engaged: false
    property bool degraded: false
    property string responsibilityText: "Driver"

    // ACC data
    property double setSpeed: 0
    property double followingGap: 2.0
    property string featureSummary: ""
    property bool accActive: false
    property bool lkaActive: false
    property bool lcaActive: false

    // DMS
    property bool showHandsOnReminder: false

    // Limitations
    property var limitations: []

    width: 280
    height: 200
    radius: 12
    color: getBackgroundColor()
    border.color: getBorderColor()
    border.width: 2

    // State-based background color
    function getBackgroundColor() {
        if (root.degraded) return "#3D0000"   // errorBackground
        if (root.engaged) return "#003D00"    // successBackground
        if (root.available) return "#00263D"  // infoBackground
        return "#1A1A1A"                       // surfaceSecondary
    }

    // State-based border color
    function getBorderColor() {
        if (root.degraded) return "#EF4444"   // error
        if (root.engaged) return "#22C55E"    // success
        if (root.available) return "#60A5FA"  // info
        return "#333333"                       // border
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16  // Spacing.medium
        spacing: 8           // Spacing.small

        // State header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8  // Spacing.small

            // State icon
            Rectangle {
                width: 32
                height: 32
                radius: 16
                color: root.engaged ? "#22C55E" :
                       (root.available ? "#60A5FA" : "#2A2A2A")

                Text {
                    anchors.centerIn: parent
                    text: root.engaged ? "A" : (root.available ? "R" : "O")
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }
            }

            // State text
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: root.hmiStateText
                    font.pixelSize: 18
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }

                Text {
                    text: root.responsibilityText
                    font.pixelSize: 11
                    color: "#808080"
                }
            }

            // Automation level badge
            Rectangle {
                visible: root.automationLevel > 0
                width: 36
                height: 24
                radius: 4
                color: "#3B82F6"  // primary

                Text {
                    anchors.centerIn: parent
                    text: "L" + root.automationLevel
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }
            }
        }

        // Active features
        RowLayout {
            Layout.fillWidth: true
            spacing: 8  // Spacing.small
            visible: root.engaged

            FeatureBadge {
                text: "ACC"
                active: root.accActive
            }

            FeatureBadge {
                text: "LKA"
                active: root.lkaActive
            }

            FeatureBadge {
                text: "LCA"
                active: root.lcaActive
            }
        }

        // Set speed and following gap (when ACC active)
        RowLayout {
            Layout.fillWidth: true
            spacing: 16  // Spacing.medium
            visible: root.accActive && root.engaged

            ColumnLayout {
                spacing: 2

                Text {
                    text: "SET"
                    font.pixelSize: 11
                    color: "#808080"
                }

                Text {
                    text: Math.round(root.setSpeed) + " km/h"
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }
            }

            Rectangle {
                width: 1
                height: 40
                color: "#333333"  // border
            }

            ColumnLayout {
                spacing: 2

                Text {
                    text: "GAP"
                    font.pixelSize: 11
                    color: "#808080"
                }

                Row {
                    spacing: 4
                    Repeater {
                        model: 4
                        Rectangle {
                            width: 8
                            height: 16
                            radius: 2
                            color: index < getGapBars() ? "#22C55E" : "#2A2A2A"
                        }
                    }
                }
            }
        }

        // Hands-on reminder
        Rectangle {
            Layout.fillWidth: true
            height: 32
            radius: 4
            color: "#3D2600"  // warningBackground
            visible: root.showHandsOnReminder && root.engaged

            Row {
                anchors.centerIn: parent
                spacing: 8  // Spacing.small

                Text {
                    text: "!"
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    color: "#F59E0B"  // warning
                }

                Text {
                    text: "Keep hands on wheel"
                    font.pixelSize: 12
                    color: "#F59E0B"  // warning
                }
            }

            // Blinking animation
            SequentialAnimation on opacity {
                running: root.showHandsOnReminder
                loops: Animation.Infinite
                NumberAnimation { to: 0.5; duration: 500 }
                NumberAnimation { to: 1.0; duration: 500 }
            }
        }

        // Limitations (if any)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            visible: root.limitations.length > 0

            Repeater {
                model: Math.min(root.limitations.length, 2)

                Text {
                    text: root.limitations[index]
                    font.pixelSize: 11
                    color: "#F59E0B"  // warning
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Item { Layout.fillHeight: true }
    }

    // Helper function for gap bars
    function getGapBars() {
        if (root.followingGap <= 1.0) return 1
        if (root.followingGap <= 1.5) return 2
        if (root.followingGap <= 2.5) return 3
        return 4
    }

    // Feature badge component
    component FeatureBadge: Rectangle {
        property string text: ""
        property bool active: false

        width: 48
        height: 28
        radius: 4
        color: active ? "#22C55E" : "#2A2A2A"
        border.color: active ? "#22C55E" : "#333333"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: parent.text
            font.pixelSize: 10
            font.weight: Font.Medium
            color: active ? "#FFFFFF" : "#808080"
        }
    }
}
