// HomePage.qml
// Home page with quick access tiles

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

Item {
    id: root

    component MetricCard: Rectangle {
        property string title: ""
        property string subtitle: ""
        property string value: ""
        property string icon: ""
        property color accent: "#4DC9CB"
        property bool interactive: false
        signal clicked

        color: "#111821"
        radius: 28
        border.color: "#1C2832"
        border.width: 1
        height: 180

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 12

            Row {
                spacing: 12

                Rectangle {
                    width: 44
                    height: 44
                    radius: 16
                    color: accent

                    Text {
                        anchors.centerIn: parent
                        text: icon
                        font.pixelSize: 22
                        color: "#05161E"
                    }
                }

                Column {
                    spacing: 4

                    Text {
                        text: title
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        color: "#E8F3FB"
                    }

                    Text {
                        text: subtitle
                        font.pixelSize: 12
                        color: "#7F95A6"
                        width: parent.width - 80
                        elide: Text.ElideRight
                    }
                }
            }

            Text {
                text: value
                font.pixelSize: 24
                font.weight: Font.Medium
                color: "#F6FBFF"
            }

            Item {
                Layout.fillHeight: true
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: interactive
            hoverEnabled: interactive
            cursorShape: interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: parent.clicked()
        }
    }

    component QuickAction: Rectangle {
        property string label: ""
        property string icon: ""
        signal clicked

        width: 120
        height: 48
        radius: 24
        color: "#121D25"
        border.color: "#1E2A32"
        border.width: 1

        Row {
            anchors.centerIn: parent
            spacing: 8

            Text {
                text: icon
                font.pixelSize: 16
                color: "#E5F3FF"
            }

            Text {
                text: label
                font.pixelSize: 12
                font.weight: Font.Medium
                font.letterSpacing: 1
                color: "#7F95A6"
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 8
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            id: homeLayout
            width: parent.width
            spacing: 16

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 230
                radius: 32
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "#133749"
                    }
                    GradientStop {
                        position: 0.6
                        color: "#0B1924"
                    }
                    GradientStop {
                        position: 1.0
                        color: "#08111A"
                    }
                }
                border.color: "#1F3A4A"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 32
                    spacing: 24

                    Column {
                        spacing: 10
                        Layout.fillWidth: true

                        Text {
                            text: "Hello, driver"
                            font.pixelSize: 18
                            color: "#BFD5E8"
                        }

                        Text {
                            text: navController.active ? "Next stop · " + navController.destination : "Tap to set a destination"
                            font.pixelSize: 34
                            font.weight: Font.DemiBold
                            color: "#F8FCFF"
                        }

                        Text {
                            text: "Route overview · " + (navController.active ? Math.round(navController.remainingDistance / 1000) + " km remaining" : "no trip in progress")
                            font.pixelSize: 14
                            color: "#99B2C8"
                        }

                        Row {
                            spacing: 16

                            QuickAction {
                                label: "Start Trip"
                                icon: "➔"
                                onClicked: appFramework.navigateTo(2)
                            }

                            QuickAction {
                                label: "App Drawer"
                                icon: "▦"
                                onClicked: appFramework.navigateTo(7)
                            }
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 320
                        Layout.fillHeight: true
                        radius: 24
                        color: "#0D1C25"
                        border.color: "#1C2A34"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 12

                            Text {
                                text: "Journey snapshot"
                                font.pixelSize: 14
                                color: "#6F8797"
                            }

                            Row {
                                spacing: 16

                                Column {
                                    spacing: 4
                                    Text {
                                        text: "SOC"
                                        font.pixelSize: 12
                                        color: "#6F8797"
                                    }
                                    Text {
                                        text: "82%"
                                        font.pixelSize: 24
                                        font.weight: Font.Medium
                                        color: "#F5FBFF"
                                    }
                                }

                                Column {
                                    spacing: 4
                                    Text {
                                        text: "Range"
                                        font.pixelSize: 12
                                        color: "#6F8797"
                                    }
                                    Text {
                                        text: "301 mi"
                                        font.pixelSize: 24
                                        font.weight: Font.Medium
                                        color: "#F5FBFF"
                                    }
                                }

                                Column {
                                    spacing: 4
                                    Text {
                                        text: "Temp"
                                        font.pixelSize: 12
                                        color: "#6F8797"
                                    }
                                    Text {
                                        text: vehicleController.hvacTempDriver + "°C"
                                        font.pixelSize: 24
                                        font.weight: Font.Medium
                                        color: "#F5FBFF"
                                    }
                                }
                            }

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: "#1F2D37"
                            }

                            Text {
                                text: projectionController.statusBanner
                                font.pixelSize: 13
                                color: "#9AB8CC"
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 380
                spacing: 20

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 16

                    MetricCard {
                        Layout.fillWidth: true
                        title: "Navigation"
                        subtitle: navController.active ? navController.destination : "No destination"
                        value: navController.active ? Math.round(navController.remainingDistance / 1000) + " km remaining" : "Set a route"
                        icon: "🗺"
                        accent: "#4DC9CB"
                        interactive: true
                        onClicked: appFramework.navigateTo(2)
                    }

                    MetricCard {
                        Layout.fillWidth: true
                        title: "Phone Apps"
                        subtitle: projectionController.activeProjection === "None" ? "Ready for CarPlay / Android Auto" : projectionController.activeProjection
                        value: projectionController.statusBanner
                        icon: "📲"
                        accent: "#F472B6"
                        interactive: true
                        onClicked: appFramework.navigateTo(4)
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 16

                    MetricCard {
                        Layout.fillWidth: true
                        title: "Now Playing"
                        subtitle: mediaController.artist || "Select a source"
                        value: mediaController.title || "No media playing"
                        icon: "🎵"
                        accent: "#5C7AEA"
                        interactive: true
                        onClicked: appFramework.navigateTo(1)
                    }

                    MetricCard {
                        Layout.fillWidth: true
                        title: "Cabin Climate"
                        subtitle: "Driver " + vehicleController.hvacTempDriver + "° · Passenger " + vehicleController.hvacTempPassenger + "°"
                        value: vehicleController.hvacAcOn ? "Comfort mode" : "Climate off"
                        icon: "❄"
                        accent: "#5AD4E0"
                        interactive: true
                        onClicked: appFramework.navigateTo(5)
                    }
                }
            }

            Row {
                Layout.fillWidth: true
                spacing: 12

                QuickAction {
                    label: "Drive"
                    icon: "⚡"
                    onClicked: appFramework.navigateTo(5)
                }

                QuickAction {
                    label: "Camera"
                    icon: "📷"
                }

                QuickAction {
                    label: "Camp"
                    icon: "🏕"
                }

                QuickAction {
                    label: "Settings"
                    icon: "⚙"
                    onClicked: appFramework.navigateTo(6)
                }

                Item {
                    width: 1
                    Layout.fillWidth: true
                }
            }
        }
    }
}
