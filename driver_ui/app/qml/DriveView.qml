// DriveView.qml
// Central drive view with speed and gear
// Safety: Display-only, all values from validated C++ model

import QtQuick 2.15
import QtQuick.Shapes 1.15

Item {
    id: root

    width: 500
    height: 500

    // Speed gauge arc background
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4

        ShapePath {
            strokeWidth: 16
            strokeColor: "#2A2A2A"
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 24
                radiusY: root.height / 2 - 24
                startAngle: -225
                sweepAngle: 270
            }
        }
    }

    // Speed gauge arc active
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4
        visible: clusterViewModel.speedValid

        ShapePath {
            strokeWidth: 16
            strokeColor: getSpeedColor()
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            function getSpeedColor() {
                var percent = clusterViewModel.speedPercent;
                if (percent > 0.9)
                    return "#EF4444";
                if (percent > 0.7)
                    return "#F59E0B";
                return "#60A5FA";
            }

            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 24
                radiusY: root.height / 2 - 24
                startAngle: -225
                sweepAngle: 270 * clusterViewModel.speedPercent
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }

    // Tick marks
    Repeater {
        model: 9  // 0, 25, 50, 75, 100, 125, 150, 175, 200

        Item {
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            rotation: -135 + (index * 33.75)

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                width: index % 2 === 0 ? 3 : 2
                height: index % 2 === 0 ? 20 : 12
                color: {
                    var threshold = index / 8;
                    if (threshold > 0.9)
                        return "#EF4444";
                    if (threshold > 0.7)
                        return "#F59E0B";
                    return "#666666";
                }
                radius: 1
            }
        }
    }

    // Speed display with dual units
    Column {
        anchors.centerIn: parent
        spacing: 4

        // Primary speed number (current unit)
        Text {
            id: speedText
            anchors.horizontalCenter: parent.horizontalCenter
            text: clusterViewModel.speedDisplay
            font.pixelSize: 120
            font.weight: Font.Light
            font.family: "SF Pro Display, Inter, sans-serif"
            color: clusterViewModel.speedValid ? "#FFFFFF" : "#666666"

            // Stale indicator animation
            opacity: staleOpacity
            property real staleOpacity: 1.0

            SequentialAnimation on staleOpacity {
                running: clusterViewModel.speedStale
                loops: Animation.Infinite
                NumberAnimation {
                    to: 0.3
                    duration: 300
                }
                NumberAnimation {
                    to: 1.0
                    duration: 300
                }
            }
        }

        // Primary unit
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: clusterViewModel.speedUnit
            font.pixelSize: 20
            font.weight: Font.Medium
            color: "#FFFFFF"
        }

        // Secondary speed (converted unit)
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Text {
                text: getSecondarySpeed()
                font.pixelSize: 24
                font.weight: Font.Normal
                color: "#808080"

                function getSecondarySpeed() {
                    if (!clusterViewModel.speedValid)
                        return "—";
                    var speed = clusterViewModel.speed;
                    var isKmh = clusterViewModel.speedUnit === "km/h";

                    if (isKmh) {
                        // Convert km/h to mph
                        var mph = speed * 0.621371;
                        return Math.round(mph);
                    } else {
                        // Convert mph to km/h
                        var kmh = speed * 1.60934;
                        return Math.round(kmh);
                    }
                }
            }

            Text {
                text: getSecondaryUnit()
                font.pixelSize: 16
                font.weight: Font.Normal
                color: "#666666"

                function getSecondaryUnit() {
                    var isKmh = clusterViewModel.speedUnit === "km/h";
                    return isKmh ? "mph" : "km/h";
                }
            }
        }
    }

    // Gear indicator
    GearIndicator {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
    }

    // Stale/Invalid overlay
    Rectangle {
        anchors.centerIn: parent
        width: 200
        height: 80
        radius: 8
        color: "#CC000000"
        visible: !clusterViewModel.speedValid || clusterViewModel.speedStale
        opacity: clusterViewModel.speedStale ? 0.9 : 1.0

        Column {
            anchors.centerIn: parent
            spacing: 4

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: clusterViewModel.speedStale ? "SIGNAL STALE" : "NO SIGNAL"
                font.pixelSize: 16
                font.weight: Font.Bold
                color: "#FF4444"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Drive with caution"
                font.pixelSize: 12
                color: "#AAAAAA"
            }
        }
    }
}
