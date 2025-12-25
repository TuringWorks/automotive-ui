// AppDrawer.qml
// App drawer / launcher with premium design

import QtQuick 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Item {
    id: root

    // Background with subtle gradient
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#0A0F14"
            }
            GradientStop {
                position: 0.5
                color: "#0F0F0F"
            }
            GradientStop {
                position: 1.0
                color: "#08090B"
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 28

        // Header with improved typography
        Column {
            spacing: 8

            Text {
                text: "All Apps"
                font.pixelSize: 32
                font.weight: Font.Bold
                color: "#F8FAFC"
                font.letterSpacing: -0.5
            }

            Text {
                text: "Tap an app to launch"
                font.pixelSize: 14
                color: "#7F95A6"
                font.letterSpacing: 0.3
            }
        }

        // App Grid with improved spacing
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 4
            rowSpacing: 20
            columnSpacing: 20

            Repeater {
                model: appsModel

                Rectangle {
                    id: appCard
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140
                    color: appArea.containsMouse ? "#1F2830" : "#111821"
                    radius: 20
                    border.width: 1
                    border.color: appArea.containsMouse ? "#2A3844" : "#1C2832"

                    // Smooth color transition
                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }

                    Behavior on border.color {
                        ColorAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }

                    // Smooth scale animation on hover
                    scale: appArea.containsMouse ? 1.02 : 1.0
                    Behavior on scale {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }

                    // Subtle shadow
                    layer.enabled: true
                    layer.effect: DropShadow {
                        horizontalOffset: 0
                        verticalOffset: appArea.containsMouse ? 8 : 4
                        radius: appArea.containsMouse ? 16 : 12
                        samples: 17
                        color: "#00000060"

                        Behavior on verticalOffset {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }

                        Behavior on radius {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    Column {
                        anchors.centerIn: parent
                        spacing: 14

                        // Icon container with glow effect
                        Rectangle {
                            width: 56
                            height: 56
                            radius: 16
                            color: modelData.color
                            anchors.horizontalCenter: parent.horizontalCenter

                            // Subtle scale on hover
                            scale: appArea.containsMouse ? 1.08 : 1.0
                            Behavior on scale {
                                NumberAnimation {
                                    duration: 200
                                    easing.type: Easing.OutCubic
                                }
                            }

                            // Icon glow effect
                            layer.enabled: true
                            layer.effect: DropShadow {
                                horizontalOffset: 0
                                verticalOffset: 0
                                radius: 12
                                samples: 17
                                color: Qt.rgba(Qt.colorEqual(modelData.color, "#60A5FA") ? 0.38 : 0, Qt.colorEqual(modelData.color, "#60A5FA") ? 0.65 : 0, Qt.colorEqual(modelData.color, "#60A5FA") ? 0.98 : 0, appArea.containsMouse ? 0.5 : 0.3)
                            }

                            Text {
                                anchors.centerIn: parent
                                text: modelData.icon
                                font.pixelSize: 28
                            }
                        }

                        // App name with better typography
                        Text {
                            text: modelData.name
                            font.pixelSize: 15
                            font.weight: Font.DemiBold
                            color: appArea.containsMouse ? "#FFFFFF" : "#E8F3FB"
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.letterSpacing: 0.2

                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                    easing.type: Easing.OutCubic
                                }
                            }
                        }
                    }

                    MouseArea {
                        id: appArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            // Add subtle click animation
                            appCard.scale = 0.96;
                            clickTimer.start();
                            appFramework.navigateTo(modelData.page);
                        }

                        Timer {
                            id: clickTimer
                            interval: 100
                            onTriggered: appCard.scale = 1.0
                        }
                    }
                }
            }
        }

        // Footer with quick stats
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: 16
            color: "#0D1419"
            border.width: 1
            border.color: "#1A2229"

            Row {
                anchors.centerIn: parent
                spacing: 32

                Column {
                    spacing: 4

                    Text {
                        text: "9 Apps"
                        font.pixelSize: 16
                        font.weight: Font.Medium
                        color: "#F4F7FB"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "Available"
                        font.pixelSize: 11
                        color: "#7F95A6"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                Rectangle {
                    width: 1
                    height: 32
                    color: "#1C2832"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    spacing: 4

                    Text {
                        text: "Swipe down to close"
                        font.pixelSize: 12
                        color: "#7F95A6"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
    }

    property var appsModel: [
        {
            name: "Media",
            icon: "🎵",
            color: "#60A5FA",
            page: 1
        },
        {
            name: "Navigation",
            icon: "🗺",
            color: "#22C55E",
            page: 2
        },
        {
            name: "Phone",
            icon: "📞",
            color: "#A78BFA",
            page: 3
        },
        {
            name: "Phone Apps",
            icon: "📲",
            color: "#F472B6",
            page: 4
        },
        {
            name: "Vehicle",
            icon: "🚗",
            color: "#F59E0B",
            page: 5
        },
        {
            name: "Settings",
            icon: "⚙",
            color: "#808080",
            page: 6
        },
        {
            name: "Calendar",
            icon: "📅",
            color: "#EF4444",
            page: 0
        },
        {
            name: "Weather",
            icon: "🌤",
            color: "#06B6D4",
            page: 0
        },
        {
            name: "Podcasts",
            icon: "🎙",
            color: "#EC4899",
            page: 0
        }
    ]
}
