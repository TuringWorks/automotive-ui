// ProgressBar.qml
// Progress indicator bar

import QtQuick 2.15
import "../DesignSystem"

Item {
    id: root

    property real value: 0  // 0.0 to 1.0
    property real minValue: 0
    property real maxValue: 1
    property bool indeterminate: false
    property color trackColor: Colors.clusterGaugeTrack
    property color progressColor: Colors.primary

    height: 4
    width: 200

    readonly property real normalizedValue: (value - minValue) / (maxValue - minValue)

    // Track
    Rectangle {
        anchors.fill: parent
        radius: height / 2
        color: root.trackColor
    }

    // Progress
    Rectangle {
        id: progressRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: root.indeterminate ? parent.width * 0.3 : parent.width * root.normalizedValue
        radius: height / 2
        color: root.progressColor

        Behavior on width {
            enabled: !root.indeterminate
            NumberAnimation { duration: Animations.normal }
        }

        // Indeterminate animation
        SequentialAnimation on x {
            running: root.indeterminate
            loops: Animation.Infinite
            NumberAnimation {
                from: -progressRect.width
                to: root.width
                duration: 1500
                easing.type: Easing.InOutQuad
            }
        }
    }
}
