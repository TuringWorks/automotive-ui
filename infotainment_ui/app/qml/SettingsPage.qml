// SettingsPage.qml
// System settings page

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: "#0F0F0F"
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 24
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 24

            Text {
                text: "Settings"
                font.pixelSize: 24
                font.weight: Font.DemiBold
                color: "#FFFFFF"
            }

            ListView {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(settingsModel.length * 72, 500)
                clip: true
                spacing: 8
                model: settingsModel
                interactive: false

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 64
                    radius: 12
                    color: settingArea.containsMouse ? "#262626" : "#1A1A1A"

                    Row {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 16

                        Rectangle {
                            width: 40
                            height: 40
                            radius: 12
                            color: modelData.color + "20"
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                anchors.centerIn: parent
                                text: modelData.icon
                                font.pixelSize: 18
                            }
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 4

                            Text {
                                text: modelData.title
                                font.pixelSize: 16
                                font.weight: Font.Medium
                                color: "#FFFFFF"
                            }

                            Text {
                                text: modelData.subtitle
                                font.pixelSize: 12
                                color: "#808080"
                            }
                        }

                        Item {
                            width: parent.width - 250
                            height: 1
                        }

                        Text {
                            text: "›"
                            font.pixelSize: 24
                            color: "#808080"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        id: settingArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }

            // Permissions section
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: permColumn.height + 32
                color: "#1A1A1A"
                radius: 16

                Column {
                    id: permColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                    spacing: 16

                    Text {
                        text: "Permissions"
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        color: "#FFFFFF"
                    }

                    Text {
                        text: "Granted permissions:"
                        font.pixelSize: 12
                        color: "#808080"
                    }

                    Flow {
                        width: parent.width
                        spacing: 8

                        Repeater {
                            model: permissionManager.grantedPermissions

                            Rectangle {
                                width: permText.implicitWidth + 16
                                height: 28
                                radius: 14
                                color: "#22C55E20"

                                Text {
                                    id: permText
                                    anchors.centerIn: parent
                                    text: modelData
                                    font.pixelSize: 12
                                    color: "#22C55E"
                                }
                            }
                        }
                    }
                }
            }

            // Version info
            Text {
                text: "Infotainment UI v1.0.0"
                font.pixelSize: 12
                color: "#808080"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    property var settingsModel: [
        {
            title: "Display",
            subtitle: "Brightness, theme",
            icon: "🖥",
            color: "#60A5FA"
        },
        {
            title: "Sound",
            subtitle: "Volume, equalizer",
            icon: "🔊",
            color: "#A78BFA"
        },
        {
            title: "Connectivity",
            subtitle: "Bluetooth, Wi-Fi",
            icon: "📶",
            color: "#22C55E"
        },
        {
            title: "Language",
            subtitle: "English (US)",
            icon: "🌐",
            color: "#F59E0B"
        },
        {
            title: "Privacy",
            subtitle: "Data, permissions",
            icon: "🔒",
            color: "#EF4444"
        },
        {
            title: "About",
            subtitle: "System info, updates",
            icon: "ℹ",
            color: "#06B6D4"
        }
    ]
}
