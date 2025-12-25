// GearIndicator.qml
// PRNDL gear indicator
// Safety: Display-only

import QtQuick 2.15

Row {
    id: root

    spacing: 16

    property var gears: ["P", "R", "N", "D"]

    Repeater {
        model: root.gears

        Rectangle {
            width: 48
            height: 48
            radius: 8
            color: isActive ? "#60A5FA" : "transparent"
            border.color: isActive ? "#60A5FA" : "#444444"
            border.width: 2

            property bool isActive: clusterViewModel.gear === modelData

            Text {
                anchors.centerIn: parent
                text: modelData
                font.pixelSize: 24
                font.weight: isActive ? Font.Bold : Font.Normal
                color: isActive ? "#000000" : "#808080"
            }

            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
    }

    // Invalid indicator
    Rectangle {
        width: 48
        height: 48
        radius: 8
        color: "#FF4444"
        visible: !clusterViewModel.gearValid

        Text {
            anchors.centerIn: parent
            text: "?"
            font.pixelSize: 24
            font.weight: Font.Bold
            color: "#FFFFFF"
        }

        SequentialAnimation on opacity {
            running: !clusterViewModel.gearValid
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 300 }
            NumberAnimation { to: 1.0; duration: 300 }
        }
    }
}
