// AppShell.qml
// Main app shell with navigation

import QtQuick 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Item {
    id: root

    property string currentTime: Qt.formatTime(new Date(), "hh:mm")

    Timer {
        interval: 60000
        running: true
        repeat: true
        onTriggered: currentTime = Qt.formatTime(new Date(), "hh:mm")
    }

    // Navigation rail
    NavigationRail {
        id: navRail
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: nowPlayingBar.top
        anchors.margins: 16
        width: 96
    }

    // Content column
    ColumnLayout {
        id: contentColumn
        anchors.left: navRail.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: nowPlayingBar.top
        anchors.margins: 32
        spacing: 24

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            radius: 32
            color: "#101922CC"
            border.width: 1
            border.color: "#20303D"
            layer.enabled: true
            layer.effect: DropShadow {
                horizontalOffset: 0
                verticalOffset: 10
                radius: 40
                samples: 25
                color: "#00000080"
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 28
                anchors.rightMargin: 28
                spacing: 24

                Column {
                    spacing: 4
                    Layout.preferredWidth: 280

                    Text {
                        text: "Lat " + navController.latitude.toFixed(4) + "°, Lon " + navController.longitude.toFixed(4) + "°"
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        color: "#F4F7FB"
                    }

                    Text {
                        text: navController.active ? ("Guiding to " + navController.destination) : "GPS signal live"
                        font.pixelSize: 12
                        color: "#8AA3B8"
                    }
                }

                Rectangle {
                    Layout.preferredHeight: parent.height - 24
                    Layout.preferredWidth: 1
                    color: "#1E2B38"
                }

                Row {
                    spacing: 16
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Column {
                        spacing: 2

                        Text {
                            text: "State of Charge"
                            font.pixelSize: 12
                            color: "#7E94AA"
                        }

                        Text {
                            text: "82%"
                            font.pixelSize: 20
                            font.weight: Font.Medium
                            color: "#F4F7FB"
                        }
                    }

                    Column {
                        spacing: 2

                        Text {
                            text: "Range"
                            font.pixelSize: 12
                            color: "#7E94AA"
                        }

                        Text {
                            text: "301 mi"
                            font.pixelSize: 20
                            font.weight: Font.Medium
                            color: "#F4F7FB"
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Column {
                        spacing: 2

                        Text {
                            text: "Drive mode"
                            font.pixelSize: 12
                            color: "#7E94AA"
                            horizontalAlignment: Text.AlignRight
                            width: 120
                        }

                        Text {
                            text: vehicleController.driveMode
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                            color: "#F4F7FB"
                            horizontalAlignment: Text.AlignRight
                            width: 120
                        }
                    }

                    Rectangle {
                        width: 2
                        radius: 1
                        color: "#1E2B38"
                        Layout.preferredHeight: parent.height - 24
                    }

                    Column {
                        spacing: 2

                        Text {
                            text: "Time"
                            font.pixelSize: 12
                            color: "#7E94AA"
                            horizontalAlignment: Text.AlignRight
                            width: 60
                        }

                        Text {
                            text: currentTime
                            font.pixelSize: 20
                            font.weight: Font.Medium
                            color: "#F4F7FB"
                            horizontalAlignment: Text.AlignRight
                            width: 60
                        }
                    }
                }
            }
        }

        Loader {
            id: pageLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: getPageSource(appFramework.currentPage)

            function getPageSource(page) {
                switch(page) {
                    case 0: return "HomePage.qml"
                    case 1: return "MediaPage.qml"
                    case 2: return "NavigationPage.qml"
                    case 3: return "PhonePage.qml"
                    case 4: return "ProjectionPage.qml"
                    case 5: return "VehiclePage.qml"
                    case 6: return "autonomy/AutonomyCenterPage.qml"
                    case 7: return "SettingsPage.qml"
                    case 8: return "AppDrawer.qml"
                    default: return "HomePage.qml"
                }
            }
        }
    }

    // Now playing bar
    NowPlayingBar {
        id: nowPlayingBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 92
        visible: mediaController.playing || mediaController.title !== ""
    }
}
