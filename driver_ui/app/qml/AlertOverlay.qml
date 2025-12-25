// AlertOverlay.qml
// Alert display overlay
// Safety: SR-CL-003 - Critical alerts cannot be occluded

import QtQuick 2.15

Item {
    id: root

    height: alertColumn.height
    visible: clusterViewModel.hasAlerts

    Column {
        id: alertColumn
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 8

        Repeater {
            model: clusterViewModel.alerts

            Rectangle {
                id: alertItem
                width: parent.width
                height: alertContent.height + 32
                radius: 8

                color: getAlertColor(modelData.priority)
                border.color: getAlertBorderColor(modelData.priority)
                border.width: modelData.priority <= 1 ? 2 : 1

                function getAlertColor(priority) {
                    switch(priority) {
                        case 0: return "#DCCC0000"  // Critical - red
                        case 1: return "#DCF59E0B"  // Warning - amber
                        default: return "#DC262626" // Info - dark
                    }
                }

                function getAlertBorderColor(priority) {
                    switch(priority) {
                        case 0: return "#FF0000"
                        case 1: return "#FFB800"
                        default: return "#444444"
                    }
                }

                // Critical alert pulsing
                SequentialAnimation on border.width {
                    running: modelData.priority === 0 && !modelData.acknowledged
                    loops: Animation.Infinite
                    NumberAnimation { to: 4; duration: 300 }
                    NumberAnimation { to: 2; duration: 300 }
                }

                Row {
                    id: alertContent
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 16
                    spacing: 16

                    // Priority icon
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 16
                        color: alertItem.getAlertBorderColor(modelData.priority)
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            anchors.centerIn: parent
                            text: modelData.priority === 0 ? "!" :
                                  (modelData.priority === 1 ? "⚠" : "ℹ")
                            font.pixelSize: 16
                            font.weight: Font.Bold
                            color: "#FFFFFF"
                        }
                    }

                    // Text content
                    Column {
                        width: parent.width - 32 - 100 - 32  // icon + buttons + spacing
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 4

                        Text {
                            width: parent.width
                            text: modelData.title
                            font.pixelSize: 16
                            font.weight: Font.SemiBold
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                        }

                        Text {
                            width: parent.width
                            text: modelData.message
                            font.pixelSize: 14
                            color: "#CCCCCC"
                            elide: Text.ElideRight
                            visible: modelData.message !== ""
                        }
                    }

                    // Action buttons
                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 8

                        // Acknowledge button (for critical/warning)
                        Rectangle {
                            width: 80
                            height: 36
                            radius: 4
                            color: ackArea.pressed ? "#FFFFFF" :
                                   alertItem.getAlertBorderColor(modelData.priority)
                            visible: modelData.requiresAck && !modelData.acknowledged

                            Text {
                                anchors.centerIn: parent
                                text: "ACK"
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: ackArea.pressed ? "#000000" : "#FFFFFF"
                            }

                            MouseArea {
                                id: ackArea
                                anchors.fill: parent
                                onClicked: clusterViewModel.acknowledgeAlert(modelData.id)
                            }
                        }

                        // Dismiss button
                        Rectangle {
                            width: 36
                            height: 36
                            radius: 18
                            color: dismissArea.pressed ? "#444444" : "transparent"
                            visible: modelData.dismissable

                            Text {
                                anchors.centerIn: parent
                                text: "✕"
                                font.pixelSize: 16
                                color: "#AAAAAA"
                            }

                            MouseArea {
                                id: dismissArea
                                anchors.fill: parent
                                onClicked: clusterViewModel.dismissAlert(modelData.id)
                            }
                        }
                    }
                }
            }
        }
    }
}
