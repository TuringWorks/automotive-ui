// NavigationPage.qml
// Navigation page with OpenStreetMap

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    id: root

    // Inline component for destination item
    component DestinationItem: Rectangle {
        property string name: ""
        property string address: ""
        property bool isHome: false
        property bool isWork: false

        signal clicked

        width: parent.width
        height: 56
        radius: 8
        color: destMouseArea.containsMouse ? "#111821" : "transparent"

        Row {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 12

            Rectangle {
                width: 36
                height: 36
                radius: 8
                color: isHome ? "#22C55E" : (isWork ? "#60A5FA" : "#1F2830")
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: isHome ? "\u2302" : (isWork ? "\u2692" : "\u2316")
                    font.pixelSize: 16
                    color: "#F4F7FB"
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2
                width: parent.width - 60

                Text {
                    text: name
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    color: "#F4F7FB"
                    elide: Text.ElideRight
                    width: parent.width
                }

                Text {
                    text: address
                    font.pixelSize: 11
                    color: "#7F95A6"
                    elide: Text.ElideRight
                    width: parent.width
                }
            }
        }

        MouseArea {
            id: destMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    // Inline component for map control button
    component MapControlButton: Rectangle {
        property string icon: ""
        signal clicked

        width: 40
        height: 40
        radius: 8
        color: mapBtnMouseArea.containsMouse ? "#1F2830" : "#111821"
        border.width: 1
        border.color: "#1C2832"

        Text {
            anchors.centerIn: parent
            text: icon
            font.pixelSize: 18
            color: "#F4F7FB"
        }

        MouseArea {
            id: mapBtnMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#03090F"
    }

    // Map area
    Rectangle {
        id: mapContainer
        anchors.fill: parent
        anchors.margins: 16
        anchors.rightMargin: navPanel.width + 24
        color: "#111821"
        radius: 12
        clip: true

        Map {
            id: map
            anchors.fill: parent
            anchors.margins: 4

            plugin: Plugin {
                name: "osm"
                PluginParameter {
                    name: "osm.mapping.providersrepository.disabled"
                    value: "true"
                }
                PluginParameter {
                    name: "osm.mapping.providersrepository.address"
                    value: "https://tile.openstreetmap.org/"
                }
            }

            center: QtPositioning.coordinate(navController.latitude, navController.longitude)
            zoomLevel: navController.zoomLevel

            // Current position marker
            MapQuickItem {
                id: positionMarker
                coordinate: QtPositioning.coordinate(navController.latitude, navController.longitude)
                anchorPoint.x: positionIcon.width / 2
                anchorPoint.y: positionIcon.height / 2

                sourceItem: Rectangle {
                    id: positionIcon
                    width: 24
                    height: 24
                    radius: 12
                    color: "#3B82F6"
                    border.width: 3
                    border.color: "#FFFFFF"

                    Rectangle {
                        anchors.centerIn: parent
                        width: 8
                        height: 8
                        radius: 4
                        color: "#FFFFFF"
                    }
                }
            }

            // Route polyline
            MapPolyline {
                id: routeLine
                line.width: 5
                line.color: "#3B82F6"
                path: {
                    var coords = [];
                    for (var i = 0; i < navController.routePath.length; i++) {
                        var pt = navController.routePath[i];
                        coords.push(QtPositioning.coordinate(pt.lat, pt.lng));
                    }
                    return coords;
                }
                visible: navController.active
            }

            // Destination marker
            MapQuickItem {
                visible: navController.active && navController.routePath.length > 0
                coordinate: {
                    if (navController.routePath.length > 0) {
                        var last = navController.routePath[navController.routePath.length - 1];
                        return QtPositioning.coordinate(last.lat, last.lng);
                    }
                    return QtPositioning.coordinate(0, 0);
                }
                anchorPoint.x: destIcon.width / 2
                anchorPoint.y: destIcon.height

                sourceItem: Item {
                    id: destIcon
                    width: 24
                    height: 32

                    Rectangle {
                        width: 24
                        height: 24
                        radius: 12
                        color: "#EF4444"
                        border.width: 2
                        border.color: "#FFFFFF"
                        anchors.top: parent.top

                        Text {
                            anchors.centerIn: parent
                            text: "\u2691"
                            font.pixelSize: 12
                            color: "#FFFFFF"
                        }
                    }

                    Rectangle {
                        width: 4
                        height: 10
                        color: "#EF4444"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        // Map controls overlay
        Column {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 16
            spacing: 8

            MapControlButton {
                icon: "+"
                onClicked: navController.zoomIn()
            }

            MapControlButton {
                icon: "-"
                onClicked: navController.zoomOut()
            }

            MapControlButton {
                icon: "\u2316"
                onClicked: navController.centerOnPosition()
            }
        }

        // Navigation instruction overlay
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 16
            anchors.rightMargin: 80
            height: navOverlay.height + 24
            radius: 12
            color: "#22C55EE0"
            visible: navController.active

            Row {
                id: navOverlay
                anchors.centerIn: parent
                spacing: 16

                Rectangle {
                    width: 48
                    height: 48
                    radius: 8
                    color: "#FFFFFF20"
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "\u27A4"
                        font.pixelSize: 24
                        color: "#FFFFFF"
                        rotation: navController.nextInstruction.indexOf("right") >= 0 ? 0 : navController.nextInstruction.indexOf("left") >= 0 ? 180 : -90
                    }
                }

                Column {
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: navController.nextInstruction
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        color: "#F4F7FB"
                    }

                    Text {
                        text: "in " + navController.distanceToTurn + "m"
                        font.pixelSize: 14
                        color: "#FFFFFFB0"
                    }
                }
            }
        }

        // Bottom info bar when navigating
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 16
            height: 60
            radius: 12
            color: "#111821E0"
            visible: navController.active

            Row {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 24

                Column {
                    spacing: 2
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: "ETA"
                        font.pixelSize: 11
                        color: "#808080"
                    }
                    Text {
                        text: navController.eta
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        color: "#22C55E"
                    }
                }

                Rectangle {
                    width: 1
                    height: parent.height - 8
                    color: "#1C2832"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    spacing: 2
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: "Remaining"
                        font.pixelSize: 11
                        color: "#808080"
                    }
                    Text {
                        text: (navController.remainingDistance / 1000).toFixed(1) + " km"
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        color: "#F4F7FB"
                    }
                }

                Item {
                    width: 1
                    height: 1
                    Layout.fillWidth: true
                }

                Rectangle {
                    width: 80
                    height: 36
                    radius: 18
                    color: "#EF4444"
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "End"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: navController.stopNavigation()
                    }
                }
            }
        }
    }

    // Right panel
    Rectangle {
        id: navPanel
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 16
        width: 280
        color: "#111821"
        radius: 12

        Flickable {
            anchors.fill: parent
            anchors.margins: 12
            contentWidth: width
            contentHeight: navColumn.height
            clip: true

            Column {
                id: navColumn
                width: parent.width
                spacing: 12

                Text {
                    text: "Navigation"
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                    color: "#F4F7FB"
                }

                // Search box
                Rectangle {
                    width: parent.width
                    height: 44
                    radius: 22
                    color: "#111821"

                    Row {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 10

                        Text {
                            text: "\uD83D\uDD0D"
                            font.pixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text {
                            text: "Search destination..."
                            font.pixelSize: 13
                            color: "#7F95A6"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#1C2832"
                }

                // Recent destinations header
                Text {
                    text: "Recent Destinations"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    color: "#7F95A6"
                }

                // Recent destinations list
                ListView {
                    width: parent.width
                    height: 300
                    clip: true
                    model: navController.recentDestinations
                    spacing: 4

                    delegate: DestinationItem {
                        width: ListView.view.width
                        name: modelData.name
                        address: modelData.address
                        isHome: modelData.name === "Home"
                        isWork: modelData.name === "Work"
                        onClicked: navController.startNavigation(modelData.name)
                    }
                }

                Item {
                    width: 1
                    height: 1
                }  // Spacer

                // Active navigation card
                Rectangle {
                    width: parent.width
                    height: activeNavCol.height + 24
                    radius: 12
                    color: "#22C55E20"
                    border.color: "#22C55E"
                    border.width: 1
                    visible: navController.active

                    Column {
                        id: activeNavCol
                        anchors.centerIn: parent
                        spacing: 8
                        width: parent.width - 24

                        Row {
                            spacing: 8

                            Text {
                                text: "\u2691"
                                font.pixelSize: 14
                                color: "#22C55E"
                            }

                            Text {
                                text: "Navigating to"
                                font.pixelSize: 12
                                color: "#808080"
                            }
                        }

                        Text {
                            text: navController.destination
                            font.pixelSize: 15
                            font.weight: Font.DemiBold
                            color: "#F4F7FB"
                        }

                        Row {
                            spacing: 16

                            Column {
                                spacing: 2
                                Text {
                                    text: "ETA"
                                    font.pixelSize: 10
                                    color: "#808080"
                                }
                                Text {
                                    text: navController.eta
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    color: "#22C55E"
                                }
                            }

                            Column {
                                spacing: 2
                                Text {
                                    text: "Distance"
                                    font.pixelSize: 10
                                    color: "#808080"
                                }
                                Text {
                                    text: (navController.remainingDistance / 1000).toFixed(1) + " km"
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    color: "#F4F7FB"
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 32
                            radius: 16
                            color: "#EF4444"

                            Text {
                                anchors.centerIn: parent
                                text: "Stop Navigation"
                                font.pixelSize: 12
                                font.weight: Font.Medium
                                color: "#FFFFFF"
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: navController.stopNavigation()
                            }
                        }
                    }
                }
            }
        }
    }
}
