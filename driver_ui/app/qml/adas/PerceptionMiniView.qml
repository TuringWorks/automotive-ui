// PerceptionMiniView.qml
// Stylized Perception Mini-View for Driver UI
// Part of: Driver UI Application Layer
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.1, 4.5

import QtQuick 2.15
import QtQuick.Layouts 1.15

/**
 * Perception Mini-View - 2D stylized lane and vehicle display.
 *
 * Shows:
 * - Lane corridor with confidence indication
 * - Lead vehicle with distance
 * - Ego vehicle position
 * - Adjacent objects (simplified)
 *
 * Safety Requirements:
 * - SR-CL-ADAS-141: Lane visualization reflects quality
 * - SR-CL-ADAS-142: Hide stale object markers
 */
Rectangle {
    id: root

    // Lane data
    property bool hasValidLanes: false
    property int laneQuality: 0          // ConfidenceLevel enum
    property double lateralOffset: 0     // meters

    // Lead vehicle data
    property bool hasLeadVehicle: false
    property double leadDistance: 0      // meters
    property double leadRelativeSpeed: 0 // m/s
    property bool leadIsClosing: false
    property double timeGap: 0           // seconds

    // Object count
    property int objectCount: 0

    width: 240
    height: 180
    radius: 12
    color: "#1A1A1A"     // surfaceSecondary
    border.color: "#333333"  // border
    border.width: 1

    clip: true

    // View scale: 1 pixel = 0.5 meters
    readonly property real pixelsPerMeter: 2.0
    readonly property real viewRangeMeters: 80  // How far ahead we show

    // Confidence colors
    readonly property var confidenceColors: [
        "#666666",    // NotAvailable (textTertiary)
        "#EF4444",    // Low (error)
        "#F59E0B",    // Medium (warning)
        "#22C55E"     // High (success)
    ]

    // Road surface
    Rectangle {
        id: roadSurface
        anchors.centerIn: parent
        width: 80
        height: parent.height
        color: "#2a2a2a"
    }

    // Lane lines
    Canvas {
        id: laneCanvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            if (!root.hasValidLanes) return

            var centerX = width / 2
            var laneHalfWidth = 35  // pixels

            // Left lane line
            drawLaneLine(ctx, centerX - laneHalfWidth, root.laneQuality)

            // Right lane line
            drawLaneLine(ctx, centerX + laneHalfWidth, root.laneQuality)

            // Center dashed line
            ctx.strokeStyle = "#ffffff"
            ctx.lineWidth = 2
            ctx.setLineDash([8, 8])
            ctx.beginPath()
            ctx.moveTo(centerX, 0)
            ctx.lineTo(centerX, height)
            ctx.stroke()
            ctx.setLineDash([])
        }

        function drawLaneLine(ctx, x, quality) {
            var color = root.confidenceColors[Math.min(quality, 3)]
            ctx.strokeStyle = color
            ctx.lineWidth = quality >= 2 ? 3 : 2

            if (quality < 2) {
                // Low confidence - dashed
                ctx.setLineDash([6, 6])
            } else {
                ctx.setLineDash([])
            }

            ctx.beginPath()
            ctx.moveTo(x, 0)
            ctx.lineTo(x, height)
            ctx.stroke()
            ctx.setLineDash([])
        }

        // Repaint on data change
        Connections {
            target: root
            function onHasValidLanesChanged() { laneCanvas.requestPaint() }
            function onLaneQualityChanged() { laneCanvas.requestPaint() }
        }
    }

    // Ego vehicle (bottom center)
    Rectangle {
        id: egoVehicle
        width: 24
        height: 40
        radius: 4
        x: parent.width / 2 - width / 2 + (root.lateralOffset * root.pixelsPerMeter)
        y: parent.height - height - 10
        color: "#3B82F6"      // primary
        border.color: "#2563EB"  // primaryDark
        border.width: 2

        // Direction indicator
        Rectangle {
            width: 8
            height: 8
            radius: 4
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 4
            color: "#ffffff"
        }
    }

    // Lead vehicle
    Rectangle {
        id: leadVehicleMarker
        visible: root.hasLeadVehicle
        width: 28
        height: 44
        radius: 4
        x: parent.width / 2 - width / 2
        y: Math.max(10, parent.height - (root.leadDistance * root.pixelsPerMeter) - height)
        color: root.leadIsClosing ? "#EF4444" : "#60A5FA"  // error : info
        border.color: root.leadIsClosing ? "#B91C1C" : "#2563EB"  // errorDark : infoDark
        border.width: 2

        // Distance label
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top
            anchors.bottomMargin: 4
            width: distanceText.width + 8
            height: 16
            radius: 3
            color: "#00000080"

            Text {
                id: distanceText
                anchors.centerIn: parent
                text: Math.round(root.leadDistance) + "m"
                font.pixelSize: 11
                color: "#ffffff"
            }
        }

        // Closing indicator animation
        SequentialAnimation on opacity {
            running: root.leadIsClosing && root.hasLeadVehicle
            loops: Animation.Infinite
            NumberAnimation { to: 0.5; duration: 300 }
            NumberAnimation { to: 1.0; duration: 300 }
        }
    }

    // Time gap indicator
    Rectangle {
        visible: root.hasLeadVehicle && root.timeGap > 0
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8
        width: 50
        height: 24
        radius: 4
        color: getTimeGapColor()

        Text {
            anchors.centerIn: parent
            text: root.timeGap.toFixed(1) + "s"
            font.pixelSize: 10
            font.weight: Font.Medium
            color: "#ffffff"
        }

        function getTimeGapColor() {
            if (root.timeGap < 1.0) return "#EF4444"  // error
            if (root.timeGap < 1.5) return "#F59E0B"  // warning
            return "#22C55E"  // success
        }
    }

    // Lane quality indicator
    Row {
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8
        spacing: 4
        visible: root.hasValidLanes

        Rectangle {
            width: 8
            height: 8
            radius: 4
            color: root.confidenceColors[Math.min(root.laneQuality, 3)]
        }

        Text {
            text: getLaneQualityText()
            font.pixelSize: 11
            color: "#808080"  // textSecondary

            function getLaneQualityText() {
                switch (root.laneQuality) {
                    case 0: return "No Lane"
                    case 1: return "Poor"
                    case 2: return "Fair"
                    case 3: return "Good"
                    default: return ""
                }
            }
        }
    }

    // Object count (if any)
    Rectangle {
        visible: root.objectCount > 0
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        width: 60
        height: 20
        radius: 4
        color: "#00000060"

        Text {
            anchors.centerIn: parent
            text: root.objectCount + " obj"
            font.pixelSize: 11
            color: "#808080"  // textSecondary
        }
    }

    // "No Data" overlay when lanes invalid
    Rectangle {
        anchors.fill: parent
        color: "#00000080"
        visible: !root.hasValidLanes
        radius: parent.radius

        Text {
            anchors.centerIn: parent
            text: "Lane Detection\nUnavailable"
            font.pixelSize: 12
            color: "#808080"  // textSecondary
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
