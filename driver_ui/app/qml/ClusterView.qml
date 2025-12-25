// ClusterView.qml
// Main cluster display layout
// Safety: Display-only, data from validated C++ model

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    // Background
    Rectangle {
        anchors.fill: parent
        color: clusterViewModel.isDegraded ? "#1A0000" : "#000000"

        Behavior on color {
            ColorAnimation { duration: 300 }
        }
    }

    // Degraded mode view
    DegradedModeView {
        anchors.fill: parent
        visible: clusterViewModel.isDegraded
        z: 100
    }

    // Normal content
    Item {
        anchors.fill: parent
        opacity: clusterViewModel.isDegraded ? 0.5 : 1.0
        visible: !clusterViewModel.isDegraded || true  // Always visible in degraded

        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }

        // Left section - Secondary gauges
        Column {
            anchors.left: parent.left
            anchors.leftMargin: 48
            anchors.verticalCenter: parent.verticalCenter
            spacing: 32
            visible: clusterViewModel.showSecondaryGauges

            // Battery gauge
            Item {
                width: 160
                height: 160

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "transparent"
                    border.color: "#333333"
                    border.width: 8
                }

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "transparent"
                    border.color: getBatteryColor()
                    border.width: 8

                    function getBatteryColor() {
                        var level = clusterViewModel.batteryLevel
                        if (level < 20) return "#EF4444"
                        if (level < 40) return "#F59E0B"
                        return "#22C55E"
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: clusterViewModel.batteryValid ?
                              Math.round(clusterViewModel.batteryLevel) + "%" : "—"
                        font.pixelSize: 32
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "BATTERY"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: "#808080"
                    }
                }
            }

            // Range display
            Column {
                width: 160
                spacing: 4

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: clusterViewModel.batteryValid ?
                          Math.round(clusterViewModel.range) : "—"
                    font.pixelSize: 40
                    font.weight: Font.Light
                    color: "#FFFFFF"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "km RANGE"
                    font.pixelSize: 12
                    color: "#808080"
                }
            }
        }

        // Center - Drive View (Speed + Gear)
        DriveView {
            anchors.centerIn: parent
        }

        // Right section - Info panel
        Column {
            anchors.right: parent.right
            anchors.rightMargin: 48
            anchors.verticalCenter: parent.verticalCenter
            spacing: 24
            visible: clusterViewModel.showInfoPanel

            // Time
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: clusterViewModel.timeDisplay
                font.pixelSize: 48
                font.weight: Font.Light
                color: "#FFFFFF"
            }

            // Outside temperature
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: Math.round(clusterViewModel.outsideTemp) + "°C"
                font.pixelSize: 24
                color: "#808080"
            }

            // Power consumption
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 4

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(clusterViewModel.power) + " kW"
                    font.pixelSize: 32
                    font.weight: Font.Medium
                    color: clusterViewModel.power > 0 ? "#60A5FA" : "#22C55E"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: clusterViewModel.power > 0 ? "CONSUMING" : "REGEN"
                    font.pixelSize: 12
                    color: "#808080"
                }
            }
        }

        // Top bar - Telltales
        TelltaleBar {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
            height: 48
        }

        // Alert overlay
        AlertOverlay {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
        }
    }

    // Degraded mode border
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "#FF4444"
        border.width: clusterViewModel.isDegraded ? 4 : 0
        visible: clusterViewModel.isDegraded

        SequentialAnimation on border.width {
            running: clusterViewModel.isDegraded
            loops: Animation.Infinite
            NumberAnimation { to: 2; duration: 500 }
            NumberAnimation { to: 4; duration: 500 }
        }
    }
}
