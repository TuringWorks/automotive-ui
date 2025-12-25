// DegradedModeView.qml
// Degraded mode overlay
// Safety: SR-CL-004 - Clear indication of degraded state

import QtQuick 2.15

Rectangle {
    id: root

    color: "transparent"

    // Central warning
    Column {
        anchors.centerIn: parent
        spacing: 24
        visible: clusterViewModel.isDegraded

        // Warning icon
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 80
            height: 80
            radius: 40
            color: "#FF4444"

            Text {
                anchors.centerIn: parent
                text: "⚠"
                font.pixelSize: 48
                color: "#FFFFFF"
            }

            SequentialAnimation on scale {
                running: true
                loops: Animation.Infinite
                NumberAnimation { to: 1.1; duration: 500 }
                NumberAnimation { to: 1.0; duration: 500 }
            }
        }

        // Title
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "DEGRADED MODE"
            font.pixelSize: 32
            font.weight: Font.Bold
            color: "#FF4444"
        }

        // Message
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: clusterViewModel.degradedMessage
            font.pixelSize: 18
            color: "#FFFFFF"
            horizontalAlignment: Text.AlignHCenter
        }

        // Invalid signal count
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: clusterViewModel.invalidSignalCount + " signal(s) unavailable"
            font.pixelSize: 14
            color: "#AAAAAA"
            visible: clusterViewModel.invalidSignalCount > 0
        }

        // Instructions
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: instructionText.implicitWidth + 32
            height: 48
            radius: 8
            color: "#33FFFFFF"

            Text {
                id: instructionText
                anchors.centerIn: parent
                text: "Essential information displayed. Drive with caution."
                font.pixelSize: 14
                font.weight: Font.Medium
                color: "#FFFFFF"
            }
        }
    }

    // Corner indicators showing degraded state
    Repeater {
        model: 4

        Rectangle {
            width: 100
            height: 4
            color: "#FF4444"
            x: index < 2 ? 0 : parent.width - width
            y: index % 2 === 0 ? 0 : parent.height - height

            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation { to: 0.3; duration: 500 }
                NumberAnimation { to: 1.0; duration: 500 }
            }
        }
    }
}
