// GaugeBase.qml
// Base component for circular gauges
// Safety: Display-only, no logic

import QtQuick 2.15
import QtQuick.Shapes 1.15
import "../DesignSystem"

Item {
    id: root

    // Value properties
    property real value: 0
    property real minValue: 0
    property real maxValue: 100
    property real warningThreshold: 80
    property real criticalThreshold: 95

    // Validity (SR-CL-002)
    property bool valid: true
    property bool stale: false

    // Visual properties
    property real startAngle: -135
    property real endAngle: 135
    property real trackWidth: 12
    property real arcWidth: 12
    property color trackColor: Colors.clusterGaugeTrack
    property color activeColor: Colors.clusterGaugeActive
    property color warningColor: Colors.clusterGaugeWarning
    property color criticalColor: Colors.clusterGaugeCritical
    property color invalidColor: Colors.textTertiary

    // Computed
    readonly property real normalizedValue: Math.max(0, Math.min(1,
        (value - minValue) / (maxValue - minValue)))
    readonly property real sweepAngle: (endAngle - startAngle) * normalizedValue
    readonly property real centerX: width / 2
    readonly property real centerY: height / 2
    readonly property real radius: Math.min(width, height) / 2 - trackWidth

    // Current arc color based on value and validity
    readonly property color currentArcColor: {
        if (!valid || stale) return invalidColor
        if (value >= criticalThreshold) return criticalColor
        if (value >= warningThreshold) return warningColor
        return activeColor
    }

    width: Tokens.gaugeLarge
    height: Tokens.gaugeLarge

    // Background track
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4

        ShapePath {
            strokeWidth: root.trackWidth
            strokeColor: root.trackColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: root.centerX
                centerY: root.centerY
                radiusX: root.radius
                radiusY: root.radius
                startAngle: root.startAngle - 90
                sweepAngle: root.endAngle - root.startAngle
            }
        }
    }

    // Active arc
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4
        visible: root.normalizedValue > 0

        ShapePath {
            strokeWidth: root.arcWidth
            strokeColor: root.currentArcColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: root.centerX
                centerY: root.centerY
                radiusX: root.radius
                radiusY: root.radius
                startAngle: root.startAngle - 90
                sweepAngle: root.sweepAngle
            }
        }

        Behavior on opacity {
            NumberAnimation { duration: Animations.gaugeUpdate }
        }
    }

    // Stale indicator overlay
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: Colors.degradedBorder
        border.width: 2
        radius: width / 2
        visible: root.stale
        opacity: staleAnimation.running ? 1 : 0.5

        SequentialAnimation on opacity {
            id: staleAnimation
            running: root.stale
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 500 }
            NumberAnimation { to: 1.0; duration: 500 }
        }
    }

    // Invalid overlay
    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 0.6
        height: parent.height * 0.6
        color: Colors.withAlpha(Colors.background, 0.8)
        radius: width / 2
        visible: !root.valid

        Text {
            anchors.centerIn: parent
            text: "—"
            font.pixelSize: Typography.size4xl
            font.weight: Font.Light
            color: Colors.textTertiary
        }
    }
}
