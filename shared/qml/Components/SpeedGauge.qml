// SpeedGauge.qml
// Speed display gauge for cluster
// Safety: Display-only, no logic. Values come from validated C++ model.
// Requirement: SR-CL-001 - Speed display shall be updated at >=10Hz

import QtQuick 2.15
import "../DesignSystem"

Item {
    id: root

    // Value properties (from C++ model)
    property real speed: 0
    property real maxSpeed: 200
    property string unit: "km/h"
    property bool valid: true
    property bool stale: false

    // Visual properties
    property color speedColor: Colors.clusterSpeedText
    property color unitColor: Colors.clusterUnitText

    width: Tokens.gaugeXLarge
    height: Tokens.gaugeXLarge

    // Main gauge arc
    GaugeBase {
        id: gauge
        anchors.fill: parent
        value: root.speed
        maxValue: root.maxSpeed
        valid: root.valid
        stale: root.stale
        warningThreshold: root.maxSpeed * 0.7
        criticalThreshold: root.maxSpeed * 0.9
    }

    // Speed value display
    Column {
        anchors.centerIn: parent
        spacing: Spacing.xs

        // Speed number
        Text {
            id: speedText
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.valid ? Math.round(root.speed).toString() : "—"
            font.pixelSize: Typography.speedPrimary
            font.weight: Font.Light
            font.family: Typography.fontFamily
            color: root.valid ? root.speedColor : Colors.textTertiary

            // Smooth value animation (only when valid)
            Behavior on text {
                enabled: root.valid
            }

            // Stale indicator - pulsing opacity
            opacity: root.stale ? staleOpacity : 1.0
            property real staleOpacity: 0.5

            SequentialAnimation on staleOpacity {
                running: root.stale
                loops: Animation.Infinite
                NumberAnimation { to: 1.0; duration: 300 }
                NumberAnimation { to: 0.5; duration: 300 }
            }
        }

        // Unit label
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.unit
            font.pixelSize: Typography.unitLabel
            font.weight: Font.Medium
            font.family: Typography.fontFamily
            color: root.unitColor
            opacity: root.valid ? 1.0 : Tokens.opacityDisabled
        }
    }

    // Tick marks around the gauge
    Repeater {
        model: 9  // 0, 25, 50, ... 200

        Item {
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            rotation: -135 + (index * 33.75)  // Distribute across 270 degrees

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                width: index % 2 === 0 ? 3 : 2
                height: index % 2 === 0 ? 16 : 10
                color: {
                    if (!root.valid) return Colors.textTertiary
                    var threshold = index / 8
                    if (threshold > 0.9) return Colors.clusterGaugeCritical
                    if (threshold > 0.7) return Colors.clusterGaugeWarning
                    return Colors.textSecondary
                }
                radius: 1
            }
        }
    }

    // Speed limit indicator (optional)
    property int speedLimit: 0
    property bool showSpeedLimit: speedLimit > 0

    Rectangle {
        visible: root.showSpeedLimit
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Spacing.md
        width: 48
        height: 48
        radius: width / 2
        color: "transparent"
        border.color: Colors.telltaleRed
        border.width: 3

        Text {
            anchors.centerIn: parent
            text: root.speedLimit.toString()
            font.pixelSize: Typography.sizeLg
            font.weight: Font.Bold
            color: Colors.textPrimary
        }

        // Warning state when exceeding
        opacity: root.speed > root.speedLimit ? warningOpacity : 1.0
        property real warningOpacity: 1.0

        SequentialAnimation on warningOpacity {
            running: root.speed > root.speedLimit
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 300 }
            NumberAnimation { to: 1.0; duration: 300 }
        }
    }
}
