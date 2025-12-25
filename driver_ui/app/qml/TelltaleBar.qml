// TelltaleBar.qml
// Top telltale bar
// Safety: SR-CL-003 - Critical telltales always visible

import QtQuick 2.15

Item {
    id: root

    // Inline component for telltale icons
    component TelltaleIcon: Rectangle {
        property int priority: 2
        property bool active: false
        property bool valid: true
        property bool blinking: false
        property alias iconSource: iconImage.source
        property string iconText: ""
        property color iconColor: getTelltaleColor()

        function getTelltaleColor() {
            switch (priority) {
            case 0:
                return "#FF0000";  // Critical
            case 1:
                return "#FFB800";  // Warning
            case 2:
                return "#22C55E";  // Info
            default:
                return "#FFFFFF"; // Status
            }
        }

        width: 32
        height: 32
        radius: 4
        color: "transparent"
        opacity: active ? 1.0 : 0.3

        Image {
            id: iconImage
            anchors.fill: parent
            visible: source !== ""
            sourceSize: Qt.size(width, height)
        }

        Text {
            anchors.centerIn: parent
            text: parent.iconText
            font.pixelSize: 20
            color: parent.iconColor
            visible: parent.iconText !== "" && iconImage.source === ""
        }

        // Blinking animation
        SequentialAnimation on opacity {
            running: blinking && active
            loops: Animation.Infinite
            NumberAnimation {
                to: 0.2
                duration: 300
            }
            NumberAnimation {
                to: 1.0
                duration: 300
            }
        }
    }

    // Critical telltales (left) - Always visible per SR-CL-003
    Row {
        id: criticalRow
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 16

        Repeater {
            model: clusterViewModel.criticalTelltales

            TelltaleIcon {
                priority: modelData.priority
                active: modelData.active
                valid: modelData.valid
                blinking: modelData.blinking
                iconSource: modelData.iconPath
            }
        }
    }

    // Center - Turn signals (using dedicated components)
    Row {
        anchors.centerIn: parent
        spacing: 32

        // Left turn indicator
        TurnIndicator {
            id: leftTurnIndicator
            isLeft: true
            active: leftTurnActive
            activeColor: "#22C55E"

            property bool leftTurnActive: {
                var telltales = clusterViewModel.activeTelltales;
                for (var i = 0; i < telltales.length; i++) {
                    if (telltales[i].id.indexOf("turn_left") >= 0) {
                        return telltales[i].active;
                    }
                }
                return false;
            }
        }

        // Right turn indicator
        TurnIndicator {
            id: rightTurnIndicator
            isLeft: false
            active: rightTurnActive
            activeColor: "#22C55E"

            property bool rightTurnActive: {
                var telltales = clusterViewModel.activeTelltales;
                for (var i = 0; i < telltales.length; i++) {
                    if (telltales[i].id.indexOf("turn_right") >= 0) {
                        return telltales[i].active;
                    }
                }
                return false;
            }
        }
    }

    // Right side - Status indicators
    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 16

        // High beam
        TelltaleIcon {
            priority: 3
            active: isHighBeamActive()
            iconText: "⬤"
            iconColor: "#3B82F6"

            function isHighBeamActive() {
                for (var i = 0; i < clusterViewModel.activeTelltales.length; i++) {
                    if (clusterViewModel.activeTelltales[i].id.indexOf("high_beam") >= 0) {
                        return clusterViewModel.activeTelltales[i].active;
                    }
                }
                return false;
            }
        }

        // Low beam
        TelltaleIcon {
            priority: 3
            active: isLowBeamActive()
            iconText: "◐"
            iconColor: "#22C55E"

            function isLowBeamActive() {
                for (var i = 0; i < clusterViewModel.activeTelltales.length; i++) {
                    if (clusterViewModel.activeTelltales[i].id.indexOf("low_beam") >= 0) {
                        return clusterViewModel.activeTelltales[i].active;
                    }
                }
                return false;
            }
        }
    }
}
