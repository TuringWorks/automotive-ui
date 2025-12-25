// NavigationRail.qml
// Side navigation rail with premium design

import QtQuick 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    color: "#0B1118"
    radius: 32
    opacity: 0.96
    border.color: "#1C262F"
    border.width: 1

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: 18
        radius: 40
        samples: 32
        color: "#00000090"
    }

    // Inline component for nav buttons with enhanced animations
    component NavButton: Item {
        id: navButton
        property string icon: ""
        property string label: ""
        property bool selected: false
        signal activated

        width: parent.width
        height: 78

        Rectangle {
            id: buttonBackground
            anchors.fill: parent
            anchors.margins: 10
            radius: 24
            color: selected ? "#1F3442" : (buttonMouseArea.containsMouse ? "#151D24" : "transparent")
            border.color: selected ? "#4DC9CB" : "transparent"
            border.width: selected ? 1.5 : 0

            // Smooth transitions
            Behavior on color {
                ColorAnimation {
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }

            Behavior on border.width {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }

            // Subtle scale on hover
            scale: buttonMouseArea.containsMouse && !selected ? 1.03 : 1.0
            Behavior on scale {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }

            // Glow effect when selected
            layer.enabled: selected
            layer.effect: DropShadow {
                horizontalOffset: 0
                verticalOffset: 0
                radius: 16
                samples: 17
                color: "#4DC9CB40"
            }
        }

        Column {
            anchors.centerIn: parent
            spacing: 7

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: icon
                font.pixelSize: 28
                color: "#F5F9FC"
                opacity: selected ? 1.0 : (buttonMouseArea.containsMouse ? 0.9 : 0.7)

                // Smooth opacity transition
                Behavior on opacity {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }

                // Subtle scale on hover
                scale: buttonMouseArea.containsMouse ? 1.1 : 1.0
                Behavior on scale {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }
            }

            Rectangle {
                width: 40
                height: 4
                radius: 2
                color: "#4DC9CB"
                anchors.horizontalCenter: parent.horizontalCenter
                visible: selected

                // Smooth width animation
                Behavior on width {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.OutCubic
                    }
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: label
                font.pixelSize: 11
                font.weight: selected ? Font.DemiBold : Font.Medium
                font.letterSpacing: 1.3
                color: selected ? "#FFFFFF" : (buttonMouseArea.containsMouse ? "#9FB5C6" : "#7F95A6")

                // Smooth color transition
                Behavior on color {
                    ColorAnimation {
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }

        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                // Add click feedback
                buttonBackground.scale = 0.95;
                clickFeedbackTimer.start();
                navButton.activated();
            }

            Timer {
                id: clickFeedbackTimer
                interval: 100
                onTriggered: buttonBackground.scale = 1.0
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        // Spacer at top
        Item {
            Layout.preferredHeight: 20
        }

        Item {
            Layout.preferredHeight: 12
        }

        NavButton {
            icon: "🏠"
            label: "HOME"
            selected: appFramework.currentPage === 0
            onActivated: appFramework.navigateTo(0)
        }

        NavButton {
            icon: "🎵"
            label: "MEDIA"
            selected: appFramework.currentPage === 1
            onActivated: appFramework.navigateTo(1)
        }

        NavButton {
            icon: "🗺"
            label: "NAV"
            selected: appFramework.currentPage === 2
            onActivated: appFramework.navigateTo(2)
        }

        NavButton {
            icon: "📞"
            label: "PHONE"
            selected: appFramework.currentPage === 3
            onActivated: appFramework.navigateTo(3)
        }

        NavButton {
            icon: "📲"
            label: "APPS"
            selected: appFramework.currentPage === 4
            onActivated: appFramework.navigateTo(4)
        }

        NavButton {
            icon: "🚗"
            label: "VEHICLE"
            selected: appFramework.currentPage === 5
            onActivated: appFramework.navigateTo(5)
        }

        Item {
            Layout.fillHeight: true
        }

        NavButton {
            icon: "⚙"
            label: "SETTINGS"
            selected: appFramework.currentPage === 6
            onActivated: appFramework.navigateTo(6)
        }
    }
}
