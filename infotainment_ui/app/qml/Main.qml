// Main.qml
// Infotainment UI main window

import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: window

    width: 1360
    height: 840
    visible: true
    title: qsTr("Infotainment UI")
    color: "#03090F"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#06131F" }
            GradientStop { position: 0.45; color: "#03090F" }
            GradientStop { position: 1.0; color: "#050709" }
        }
    }

    AppShell {
        anchors.fill: parent
    }
}
