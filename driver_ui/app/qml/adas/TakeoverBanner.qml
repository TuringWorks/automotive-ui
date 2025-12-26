// TakeoverBanner.qml
// Non-Occludable Takeover Request Banner
// Part of: Driver UI Application Layer
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.3

import QtQuick 2.15
import QtQuick.Layouts 1.15

/**
 * Takeover Request Banner - Full-width, cannot be occluded.
 *
 * Safety Requirements:
 * - SR-CL-ADAS-120: Non-occludable banner with escalation
 * - SR-CL-ADAS-121: Urgency levels map to consistent patterns
 * - SR-CL-ADAS-122: "Immediate Action Required" at threshold
 * - SR-CL-ADAS-130: MRM shows persistent state
 *
 * Urgency Levels (from TakeoverUrgency enum):
 * - 0: None
 * - 1: Advisory (yellow)
 * - 2: Warning (orange)
 * - 3: Immediate (red)
 * - 4: Critical (red + pulsing)
 */
Rectangle {
    id: root

    // Data bindings
    property bool active: false
    property int urgency: 0           // TakeoverUrgency enum
    property double countdown: 0
    property string message: ""
    property bool mrmActive: false
    property bool requiresImmediateAction: false

    // Z-index must be highest to prevent occlusion
    z: 1000

    // Full width banner
    width: parent.width
    height: active ? 120 : 0
    visible: active

    color: getBackgroundColor()
    border.color: getBorderColor()
    border.width: 3

    // Clip to hide during collapse
    clip: true

    // State-based colors
    function getBackgroundColor() {
        if (root.mrmActive) return "#cc0000"
        switch (root.urgency) {
            case 1: return "#aa6600"  // Advisory - amber
            case 2: return "#cc4400"  // Warning - orange
            case 3: return "#dd0000"  // Immediate - red
            case 4: return "#ff0000"  // Critical - bright red
            default: return "#aa6600"
        }
    }

    function getBorderColor() {
        if (root.mrmActive) return "#ff0000"
        switch (root.urgency) {
            case 1: return "#ffaa00"
            case 2: return "#ff6600"
            case 3: return "#ff0000"
            case 4: return "#ff0000"
            default: return "#ffaa00"
        }
    }

    // Expand/collapse animation
    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

    // Pulsing animation for critical states
    SequentialAnimation on opacity {
        id: pulseAnimation
        running: root.urgency >= 3 && root.active && !root.mrmActive
        loops: Animation.Infinite

        NumberAnimation { to: 0.7; duration: 300 }
        NumberAnimation { to: 1.0; duration: 300 }
    }

    // Content layout
    RowLayout {
        anchors.fill: parent
        anchors.margins: 16  // Spacing.medium
        spacing: 24          // Spacing.large

        // Warning icon
        Rectangle {
            width: 80
            height: 80
            radius: 40
            color: root.mrmActive ? "#ffffff" : getBorderColor()

            Text {
                anchors.centerIn: parent
                text: root.mrmActive ? "MRM" : "!"
                font.pixelSize: root.mrmActive ? 24 : 48
                font.bold: true
                color: root.mrmActive ? "#cc0000" : "#ffffff"
            }

            // Blinking for critical
            SequentialAnimation on scale {
                running: root.urgency >= 4 && !root.mrmActive
                loops: Animation.Infinite
                NumberAnimation { to: 1.1; duration: 200 }
                NumberAnimation { to: 1.0; duration: 200 }
            }
        }

        // Message area
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8  // Spacing.small

            // Main message
            Text {
                Layout.fillWidth: true
                text: root.message
                font.pixelSize: 24
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignLeft
            }

            // Sub-message based on state
            Text {
                Layout.fillWidth: true
                text: getSubMessage()
                font.pixelSize: 14
                color: "#ffffffcc"
                visible: text.length > 0
            }
        }

        // Countdown display
        Rectangle {
            visible: root.countdown > 0 && !root.mrmActive
            width: 100
            height: 80
            radius: 8
            color: "#00000040"

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: Math.ceil(root.countdown).toString()
                    font.pixelSize: 48
                    font.bold: true
                    color: "#ffffff"
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "seconds"
                    font.pixelSize: 11
                    color: "#ffffffaa"
                }
            }

            // Countdown ring (visual indicator)
            Canvas {
                id: countdownCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    var centerX = width / 2
                    var centerY = height / 2
                    var radius = 35

                    // Background circle
                    ctx.beginPath()
                    ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI)
                    ctx.strokeStyle = "#ffffff40"
                    ctx.lineWidth = 4
                    ctx.stroke()

                    // Progress arc
                    var progress = root.countdown / 10  // Assuming 10s max
                    var startAngle = -Math.PI / 2
                    var endAngle = startAngle + (2 * Math.PI * progress)

                    ctx.beginPath()
                    ctx.arc(centerX, centerY, radius, startAngle, endAngle)
                    ctx.strokeStyle = "#ffffff"
                    ctx.lineWidth = 4
                    ctx.stroke()
                }

                // Redraw on countdown change
                Connections {
                    target: root
                    function onCountdownChanged() {
                        countdownCanvas.requestPaint()
                    }
                }
            }
        }
    }

    // Helper function for sub-message
    function getSubMessage() {
        if (root.mrmActive) {
            return "Vehicle is performing a safe stop"
        }
        switch (root.urgency) {
            case 1: return "ODD boundary approaching"
            case 2: return "Take control of the vehicle"
            case 3: return "Take control NOW"
            case 4: return "IMMEDIATE ACTION REQUIRED"
            default: return ""
        }
    }

    // Audio/haptic trigger (placeholder - connect to C++)
    onActiveChanged: {
        if (active) {
            // Trigger audio alert based on urgency
            console.log("Takeover banner activated, urgency:", urgency)
        }
    }

    onUrgencyChanged: {
        if (active && urgency >= 3) {
            // Escalation audio
            console.log("Takeover escalated to urgency:", urgency)
        }
    }
}
