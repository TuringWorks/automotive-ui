// VehiclePage.qml
// Rivian-inspired vehicle settings hub

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    component TempControl: Column {
        property string label: ""
        property int value: 0
        property int heatLevel: 0
        property bool disabled: false
        signal increase()
        signal decrease()
        signal heatSelected(int level)

        spacing: 8
        opacity: disabled ? 0.4 : 1.0

        Text { text: label; font.pixelSize: 12; color: "#7F95A6" }

        Row {
            spacing: 10

            Rectangle {
                width: 34; height: 34; radius: 12
                color: "#172330"
                Text { anchors.centerIn: parent; text: "-"; color: "#F4FBFF" }
                MouseArea { anchors.fill: parent; enabled: !disabled; onClicked: decrease() }
            }

            Text {
                text: value + "°"
                font.pixelSize: 30
                font.weight: Font.Medium
                color: "#F4FBFF"
                width: 60
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                width: 34; height: 34; radius: 12
                color: "#172330"
                Text { anchors.centerIn: parent; text: "+"; color: "#F4FBFF" }
                MouseArea { anchors.fill: parent; enabled: !disabled; onClicked: increase() }
            }
        }

        Row {
            spacing: 6
            Repeater {
                model: 3
                Rectangle {
                    width: 22; height: 22; radius: 6
                    color: (index + 1) <= heatLevel ? "#F97316" : "#1F2A35"
                    Text { anchors.centerIn: parent; text: "\u25B2"; font.pixelSize: 10; color: (index + 1) <= heatLevel ? "#1F1309" : "#6F7C88" }
                    MouseArea { anchors.fill: parent; enabled: !disabled; onClicked: heatSelected(index + 1) }
                }
            }
        }
    }

    component ToggleChip: Rectangle {
        property string label: ""
        property bool active: false
        signal toggled(bool value)

        width: 140
        height: 44
        radius: 16
        color: active ? "#4DC9CB" : "#182330"
        border.color: active ? "#7FE0E2" : "#20303D"

        Text {
            anchors.centerIn: parent
            text: label
            font.pixelSize: 13
            font.weight: Font.Medium
            color: active ? "#03181D" : "#92AABF"
        }

        MouseArea { anchors.fill: parent; onClicked: toggled(!active) }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#03070D" }
            GradientStop { position: 1.0; color: "#050E18" }
        }
    }

    Flickable {
        anchors.fill: parent
        anchors.margins: 24
        contentWidth: width
        contentHeight: contentColumn.height
        clip: true

        Column {
            id: contentColumn
            width: parent.width
            spacing: 20

            Text { text: "Vehicle"; font.pixelSize: 28; font.weight: Font.DemiBold; color: "#F2F7FC" }

            Rectangle {
                width: parent.width
                height: 260
                radius: 32
                color: "#111C28"
                border.color: "#1E2B36"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 28

                    TempControl {
                        Layout.fillWidth: true
                        label: "Driver"
                        value: vehicleController.hvacTempDriver
                        heatLevel: vehicleController.seatHeatDriver
                        onIncrease: vehicleController.setHvacTempDriver(value + 1)
                        onDecrease: vehicleController.setHvacTempDriver(value - 1)
                        onHeatSelected: function(level) {
                            vehicleController.setSeatHeatDriver(heatLevel === level ? 0 : level)
                        }
                    }

                    TempControl {
                        Layout.fillWidth: true
                        label: "Passenger"
                        value: vehicleController.hvacTempPassenger
                        heatLevel: vehicleController.seatHeatPassenger
                        disabled: vehicleController.hvacSync
                        onIncrease: if (!disabled) vehicleController.setHvacTempPassenger(value + 1)
                        onDecrease: if (!disabled) vehicleController.setHvacTempPassenger(value - 1)
                        onHeatSelected: function(level) {
                            if (!disabled)
                                vehicleController.setSeatHeatPassenger(heatLevel === level ? 0 : level)
                        }
                    }

                    Column {
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 12

                        ToggleChip {
                            label: "Auto"
                            active: vehicleController.hvacAutoMode
                            onToggled: vehicleController.setHvacAutoMode(value)
                        }

                        ToggleChip {
                            label: "A/C"
                            active: vehicleController.hvacAcOn
                            onToggled: vehicleController.setHvacAcOn(value)
                        }

                        ToggleChip {
                            label: "Sync"
                            active: vehicleController.hvacSync
                            onToggled: vehicleController.setHvacSync(value)
                        }

                        ToggleChip {
                            label: "Recirc"
                            active: vehicleController.hvacRecirculation
                            onToggled: vehicleController.setHvacRecirculation(value)
                        }

                        Row {
                            spacing: 12
                            Rectangle {
                                width: 64; height: 38; radius: 14
                                color: vehicleController.defrostFront ? "#FCD34D" : "#1B2431"
                                Text { anchors.centerIn: parent; text: "Front"; font.pixelSize: 12; color: vehicleController.defrostFront ? "#221604" : "#B6C4D8" }
                                MouseArea { anchors.fill: parent; onClicked: vehicleController.setDefrostFront(!vehicleController.defrostFront) }
                            }
                            Rectangle {
                                width: 64; height: 38; radius: 14
                                color: vehicleController.defrostRear ? "#FCD34D" : "#1B2431"
                                Text { anchors.centerIn: parent; text: "Rear"; font.pixelSize: 12; color: vehicleController.defrostRear ? "#221604" : "#B6C4D8" }
                                MouseArea { anchors.fill: parent; onClicked: vehicleController.setDefrostRear(!vehicleController.defrostRear) }
                            }
                        }
                    }
                }
            }

            RowLayout {
                width: parent.width
                spacing: 20

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 240
                    radius: 28
                    color: "#0F1824"
                    border.color: "#1A2533"

                    Column {
                        anchors.fill: parent
                        anchors.margins: 22
                        spacing: 14

                        Row {
                            spacing: 12
                            Text { text: "Drive modes"; font.pixelSize: 18; font.weight: Font.DemiBold; color: "#F4F9FF" }
                            Rectangle {
                                width: 96; height: 32; radius: 16
                                color: vehicleController.canModifyDriveMode ? "#65E0B1" : "#FBBF24"
                                Text { anchors.centerIn: parent; text: vehicleController.canModifyDriveMode ? "Unlocked" : "Locked"; font.pixelSize: 11; color: vehicleController.canModifyDriveMode ? "#032016" : "#3E2706" }
                            }
                        }

                        Text { text: "Current · " + vehicleController.driveMode; font.pixelSize: 13; color: "#7F95A6" }

                        Flow {
                            width: parent.width
                            spacing: 12
                            Repeater {
                                model: ["Sport", "All-Purpose", "Conserve", "Snow", "Off-Road"]
                                Rectangle {
                                    width: 150; height: 64; radius: 18
                                    color: vehicleController.driveMode === modelData ? "#4DC9CB" : "#1A2331"
                                    opacity: vehicleController.canModifyDriveMode ? 1.0 : 0.6
                                    Column {
                                        anchors.centerIn: parent
                                        spacing: 4
                                        Text { text: modelData; font.pixelSize: 14; font.weight: Font.Medium; color: vehicleController.driveMode === modelData ? "#03181D" : "#F4F9FF" }
                                        Text { text: vehicleController.driveMode === modelData ? "Active" : "Select"; font.pixelSize: 11; color: vehicleController.driveMode === modelData ? "#06353A" : "#7E92A5" }
                                    }
                                    MouseArea { anchors.fill: parent; enabled: vehicleController.canModifyDriveMode; onClicked: vehicleController.setDriveMode(modelData) }
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 38
                            radius: 14
                            color: "#22130B"
                            visible: !vehicleController.canModifyDriveMode

                            Row {
                                anchors.centerIn: parent
                                spacing: 10
                                Text { text: "Permission required"; font.pixelSize: 12; color: "#FBBF24" }
                                Rectangle {
                                    width: 90; height: 28; radius: 14
                                    color: "#F97316"
                                    Text { anchors.centerIn: parent; text: "Request"; font.pixelSize: 12; color: "#1F1208" }
                                    MouseArea { anchors.fill: parent; onClicked: vehicleController.requestDriveModePermission() }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 240
                    radius: 28
                    color: "#111A26"
                    border.color: "#1F2A36"

                    Column {
                        anchors.fill: parent
                        anchors.margins: 22
                        spacing: 14

                        Text { text: "Lighting"; font.pixelSize: 18; font.weight: Font.DemiBold; color: "#F4F9FF" }

                        Row {
                            spacing: 12
                            Text { text: "Auto headlights"; font.pixelSize: 13; color: "#A0B4C6" }
                            Rectangle {
                                width: 68; height: 32; radius: 16
                                color: vehicleController.lightsAuto ? "#4DC9CB" : "#1A2634"
                                Rectangle {
                                    width: 30; height: 28; radius: 14
                                    color: vehicleController.lightsAuto ? "#04252C" : "#F4F8FB"
                                    x: vehicleController.lightsAuto ? parent.width - width - 2 : 2
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                MouseArea { anchors.fill: parent; onClicked: vehicleController.setLightsAuto(!vehicleController.lightsAuto) }
                            }
                        }

                        Text { text: "Ambient brightness"; font.pixelSize: 12; color: "#7F95A6" }

                        Rectangle {
                            width: parent.width - 40
                            height: 18
                            radius: 9
                            color: "#172231"
                            border.color: "#233040"

                            Rectangle {
                                width: parent.width * (vehicleController.ambientBrightness / 100)
                                height: parent.height
                                radius: 9
                                color: "#4DC9CB"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onPressed: vehicleController.setAmbientBrightness(Math.round((mouse.x / width) * 100))
                                onPositionChanged: if (mouse.buttons === Qt.LeftButton)
                                    vehicleController.setAmbientBrightness(Math.round((mouse.x / width) * 100))
                            }
                        }

                        Text { text: vehicleController.ambientBrightness + "%"; font.pixelSize: 12; color: "#7F95A6" }
                    }
                }
            }
        }
    }
}
