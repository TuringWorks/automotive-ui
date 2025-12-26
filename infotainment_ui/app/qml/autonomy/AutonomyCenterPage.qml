// AutonomyCenterPage.qml
// Autonomy Center - ADAS Control and Status Page
// Self-contained with mock simulation

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root

    // Mock ADAS State (controlled by buttons)
    property int adasState: 0  // 0=Off, 1=Available, 2=Engaged
    property var stateNames: ["Off", "Available", "Engaged"]
    property var stateColors: ["#666666", "#3B82F6", "#22C55E"]

    // Mock data
    property int automationLevel: adasState === 2 ? 2 : 0
    property bool engaged: adasState === 2
    property double setSpeed: 100
    property double followGap: 2.0
    property bool accEnabled: true
    property bool lkaEnabled: true
    property bool lcaEnabled: false

    // Mock stats
    property int todayEngagements: 12
    property int totalMinutes: 45
    property int disengagements: 3
    property double oddCoverage: 78

    // Animated mock values
    property double mockSpeed: 95
    property double mockDistance: 42
    property int mockLaneQuality: 3

    Timer {
        interval: 100
        running: engaged
        repeat: true
        onTriggered: {
            mockSpeed = 90 + Math.sin(Date.now() / 2000) * 10
            mockDistance = 35 + Math.sin(Date.now() / 3000) * 20
            if (Math.random() < 0.02) {
                mockLaneQuality = 2 + Math.floor(Math.random() * 2)
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        ColumnLayout {
            width: scrollView.width
            spacing: 20

            // Top margin
            Item { Layout.preferredHeight: 24 }

            // Header
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                spacing: 16

                Text {
                    text: "Autonomy Center"
                    font.pixelSize: 28
                    font.weight: Font.Bold
                    color: "#FFFFFF"
                }

                Item { Layout.fillWidth: true }

                // State indicator
                Rectangle {
                    width: stateLabel.width + 40
                    height: 40
                    radius: 20
                    color: stateColors[adasState]

                    Row {
                        anchors.centerIn: parent
                        spacing: 8

                        Rectangle {
                            width: 12
                            height: 12
                            radius: 6
                            color: "#FFFFFF"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text {
                            id: stateLabel
                            text: stateNames[adasState]
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            color: "#FFFFFF"
                        }
                    }
                }
            }

            // Main content
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 600
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                spacing: 20

            // Left column - Controls
            Rectangle {
                Layout.preferredWidth: 320
                Layout.fillHeight: true
                radius: 16
                color: "#1a1a1a"
                border.color: "#333333"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    Text {
                        text: "ADAS Controls"
                        font.pixelSize: 18
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    // State buttons
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Repeater {
                            model: 3
                            Rectangle {
                                Layout.fillWidth: true
                                height: 44
                                radius: 8
                                color: adasState === index ? stateColors[index] : "#2a2a2a"
                                border.color: stateColors[index]
                                border.width: adasState === index ? 0 : 1

                                Text {
                                    anchors.centerIn: parent
                                    text: stateNames[index]
                                    font.pixelSize: 12
                                    font.weight: Font.Medium
                                    color: adasState === index ? "#FFFFFF" : stateColors[index]
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: adasState = index
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#333333"
                    }

                    // Feature toggles
                    Text {
                        text: "Features"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: "#808080"
                    }

                    FeatureToggle {
                        Layout.fillWidth: true
                        label: "Adaptive Cruise Control"
                        enabled: accEnabled
                        onToggled: accEnabled = !accEnabled
                    }

                    FeatureToggle {
                        Layout.fillWidth: true
                        label: "Lane Keep Assist"
                        enabled: lkaEnabled
                        onToggled: lkaEnabled = !lkaEnabled
                    }

                    FeatureToggle {
                        Layout.fillWidth: true
                        label: "Lane Change Assist"
                        enabled: lcaEnabled
                        onToggled: lcaEnabled = !lcaEnabled
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#333333"
                    }

                    // Set Speed
                    Text {
                        text: "Set Speed"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: "#808080"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Rectangle {
                            width: 44
                            height: 44
                            radius: 22
                            color: "#2a2a2a"
                            Text {
                                anchors.centerIn: parent
                                text: "-"
                                font.pixelSize: 24
                                color: "#FFFFFF"
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: setSpeed = Math.max(30, setSpeed - 5)
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: Math.round(setSpeed) + " km/h"
                            font.pixelSize: 32
                            font.weight: Font.Light
                            color: "#FFFFFF"
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Rectangle {
                            width: 44
                            height: 44
                            radius: 22
                            color: "#2a2a2a"
                            Text {
                                anchors.centerIn: parent
                                text: "+"
                                font.pixelSize: 24
                                color: "#FFFFFF"
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: setSpeed = Math.min(200, setSpeed + 5)
                            }
                        }
                    }

                    // Following Gap
                    Text {
                        text: "Following Gap"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: "#808080"
                    }

                    Row {
                        Layout.fillWidth: true
                        spacing: 8

                        Repeater {
                            model: 4
                            Rectangle {
                                width: (parent.width - 24) / 4
                                height: 36
                                radius: 6
                                color: followGap === (index + 1) ? "#3B82F6" : "#2a2a2a"

                                Text {
                                    anchors.centerIn: parent
                                    text: (index + 1) + "s"
                                    font.pixelSize: 14
                                    color: followGap === (index + 1) ? "#FFFFFF" : "#808080"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: followGap = index + 1
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // Center column - Live View
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: "#1a1a1a"
                border.color: "#333333"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    Text {
                        text: "Live Status"
                        font.pixelSize: 18
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    // Perception view
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 12
                        color: "#0a1520"
                        clip: true

                        // Road visualization
                        Canvas {
                            id: roadCanvas
                            anchors.fill: parent
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.clearRect(0, 0, width, height)

                                var centerX = width / 2
                                var laneWidth = 80

                                // Road surface
                                ctx.fillStyle = "#1a2530"
                                ctx.fillRect(centerX - laneWidth * 1.5, 0, laneWidth * 3, height)

                                // Lane lines
                                ctx.strokeStyle = engaged ? "#22C55E" : "#666666"
                                ctx.lineWidth = 3

                                // Left lane
                                ctx.beginPath()
                                ctx.moveTo(centerX - laneWidth, 0)
                                ctx.lineTo(centerX - laneWidth, height)
                                ctx.stroke()

                                // Right lane
                                ctx.beginPath()
                                ctx.moveTo(centerX + laneWidth, 0)
                                ctx.lineTo(centerX + laneWidth, height)
                                ctx.stroke()

                                // Center dashed line
                                ctx.strokeStyle = "#FFFFFF"
                                ctx.lineWidth = 2
                                ctx.setLineDash([15, 15])
                                ctx.beginPath()
                                ctx.moveTo(centerX, 0)
                                ctx.lineTo(centerX, height)
                                ctx.stroke()
                                ctx.setLineDash([])
                            }

                            Connections {
                                target: root
                                function onEngagedChanged() { roadCanvas.requestPaint() }
                            }
                        }

                        // Ego vehicle
                        Rectangle {
                            x: parent.width / 2 - 20
                            y: parent.height - 80
                            width: 40
                            height: 60
                            radius: 6
                            color: "#3B82F6"
                            border.color: "#2563EB"
                            border.width: 2
                        }

                        // Lead vehicle (when engaged)
                        Rectangle {
                            visible: engaged
                            x: parent.width / 2 - 22
                            y: parent.height - mockDistance * 3 - 70
                            width: 44
                            height: 65
                            radius: 6
                            color: mockDistance < 25 ? "#EF4444" : "#60A5FA"
                            border.color: mockDistance < 25 ? "#B91C1C" : "#2563EB"
                            border.width: 2

                            Text {
                                anchors.top: parent.bottom
                                anchors.topMargin: 4
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: Math.round(mockDistance) + "m"
                                font.pixelSize: 12
                                color: "#FFFFFF"
                            }
                        }

                        // Speed display
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.margins: 16
                            width: 100
                            height: 60
                            radius: 8
                            color: "#00000080"

                            Column {
                                anchors.centerIn: parent
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: Math.round(engaged ? mockSpeed : 0)
                                    font.pixelSize: 28
                                    font.weight: Font.Medium
                                    color: "#FFFFFF"
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "km/h"
                                    font.pixelSize: 12
                                    color: "#808080"
                                }
                            }
                        }

                        // Status overlay when not engaged
                        Rectangle {
                            anchors.fill: parent
                            color: "#00000060"
                            visible: !engaged
                            radius: 12

                            Text {
                                anchors.centerIn: parent
                                text: adasState === 0 ? "ADAS Off" : "Ready to Engage"
                                font.pixelSize: 20
                                color: "#808080"
                            }
                        }
                    }

                    // Feature status row
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        FeatureStatus {
                            label: "ACC"
                            active: engaged && accEnabled
                        }
                        FeatureStatus {
                            label: "LKA"
                            active: engaged && lkaEnabled
                        }
                        FeatureStatus {
                            label: "LCA"
                            active: engaged && lcaEnabled
                        }
                        Item { Layout.fillWidth: true }
                        Text {
                            text: "Lane: " + ["Poor", "Fair", "Good"][mockLaneQuality - 1]
                            font.pixelSize: 12
                            color: ["#EF4444", "#F59E0B", "#22C55E"][mockLaneQuality - 1]
                            visible: engaged
                        }
                    }
                }
            }

            // Right column - Stats
            Rectangle {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                radius: 16
                color: "#1a1a1a"
                border.color: "#333333"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    Text {
                        text: "Today's Stats"
                        font.pixelSize: 18
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    StatCard {
                        Layout.fillWidth: true
                        title: "Engagements"
                        value: todayEngagements.toString()
                        subtitle: totalMinutes + " min total"
                        accentColor: "#22C55E"
                    }

                    StatCard {
                        Layout.fillWidth: true
                        title: "Disengagements"
                        value: disengagements.toString()
                        subtitle: "Manual takeovers"
                        accentColor: "#F59E0B"
                    }

                    StatCard {
                        Layout.fillWidth: true
                        title: "ODD Coverage"
                        value: oddCoverage + "%"
                        subtitle: "On current route"
                        accentColor: "#3B82F6"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#333333"
                    }

                    Text {
                        text: "Automation Level"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: "#808080"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        radius: 12
                        color: "#2a2a2a"

                        Row {
                            anchors.centerIn: parent
                            spacing: 16

                            Text {
                                text: "L" + automationLevel
                                font.pixelSize: 48
                                font.weight: Font.Bold
                                color: engaged ? "#22C55E" : "#666666"
                            }

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    text: automationLevel === 0 ? "No Automation" :
                                          automationLevel === 1 ? "Driver Assistance" :
                                          automationLevel === 2 ? "Partial Automation" :
                                          "Conditional Automation"
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    color: "#FFFFFF"
                                }
                                Text {
                                    text: engaged ? "System Active" : "Driver Responsible"
                                    font.pixelSize: 12
                                    color: "#808080"
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }
            }

            // Bottom margin
            Item { Layout.preferredHeight: 24 }
        }
    }

    // Feature toggle component
    component FeatureToggle: Rectangle {
        property string label: ""
        property bool enabled: false
        signal toggled()

        height: 44
        radius: 8
        color: enabled ? "#1a3020" : "#2a2a2a"
        border.color: enabled ? "#22C55E" : "#333333"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12

            Text {
                text: label
                font.pixelSize: 14
                color: "#FFFFFF"
                Layout.fillWidth: true
            }

            Rectangle {
                width: 48
                height: 24
                radius: 12
                color: enabled ? "#22C55E" : "#444444"

                Rectangle {
                    x: enabled ? 26 : 2
                    y: 2
                    width: 20
                    height: 20
                    radius: 10
                    color: "#FFFFFF"

                    Behavior on x {
                        NumberAnimation { duration: 150 }
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: toggled()
        }
    }

    // Feature status badge
    component FeatureStatus: Rectangle {
        property string label: ""
        property bool active: false

        width: 56
        height: 32
        radius: 6
        color: active ? "#22C55E" : "#333333"

        Text {
            anchors.centerIn: parent
            text: label
            font.pixelSize: 12
            font.weight: Font.Medium
            color: active ? "#FFFFFF" : "#808080"
        }
    }

    // Stat card component
    component StatCard: Rectangle {
        property string title: ""
        property string value: ""
        property string subtitle: ""
        property color accentColor: "#3B82F6"

        height: 80
        radius: 12
        color: "#2a2a2a"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Rectangle {
                width: 4
                Layout.fillHeight: true
                radius: 2
                color: accentColor
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: title
                    font.pixelSize: 12
                    color: "#808080"
                }

                Text {
                    text: value
                    font.pixelSize: 28
                    font.weight: Font.Bold
                    color: "#FFFFFF"
                }

                Text {
                    text: subtitle
                    font.pixelSize: 11
                    color: "#666666"
                }
            }
        }
    }
}
