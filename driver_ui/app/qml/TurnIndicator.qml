// TurnIndicator.qml
// Animated turn indicator component
// Safety: Visual feedback for turn signal state

import QtQuick 2.15

Item {
    id: root

    property bool active: false
    property bool isLeft: true
    property color activeColor: "#22C55E"
    property color inactiveColor: "#1A1A1A"
    
    width: 120
    height: 80

    // Background container
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        radius: 8
        border.color: active ? activeColor : "#333333"
        border.width: 2
        opacity: active ? 1.0 : 0.3

        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }

        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }
    }

    // Arrow shape using Canvas
    Canvas {
        id: arrowCanvas
        anchors.centerIn: parent
        width: 60
        height: 60
        opacity: active ? 1.0 : 0.3

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            
            ctx.fillStyle = active ? activeColor : inactiveColor
            ctx.strokeStyle = active ? activeColor : inactiveColor
            ctx.lineWidth = 3
            ctx.lineJoin = "round"
            ctx.lineCap = "round"

            ctx.beginPath()
            
            if (isLeft) {
                // Left arrow: ◀
                ctx.moveTo(width * 0.7, height * 0.2)
                ctx.lineTo(width * 0.3, height * 0.5)
                ctx.lineTo(width * 0.7, height * 0.8)
                ctx.lineTo(width * 0.7, height * 0.65)
                ctx.lineTo(width * 0.5, height * 0.5)
                ctx.lineTo(width * 0.7, height * 0.35)
                ctx.closePath()
            } else {
                // Right arrow: ▶
                ctx.moveTo(width * 0.3, height * 0.2)
                ctx.lineTo(width * 0.7, height * 0.5)
                ctx.lineTo(width * 0.3, height * 0.8)
                ctx.lineTo(width * 0.3, height * 0.65)
                ctx.lineTo(width * 0.5, height * 0.5)
                ctx.lineTo(width * 0.3, height * 0.35)
                ctx.closePath()
            }
            
            ctx.fill()
        }

        // Redraw when properties change
        Connections {
            target: root
            function onActiveChanged() { arrowCanvas.requestPaint() }
            function onActiveColorChanged() { arrowCanvas.requestPaint() }
        }

        // Blinking animation when active
        SequentialAnimation on opacity {
            running: root.active
            loops: Animation.Infinite
            
            NumberAnimation {
                from: 1.0
                to: 0.2
                duration: 400
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                from: 0.2
                to: 1.0
                duration: 400
                easing.type: Easing.InOutQuad
            }
        }
    }

    // Glow effect when active
    Rectangle {
        anchors.centerIn: arrowCanvas
        width: arrowCanvas.width + 20
        height: arrowCanvas.height + 20
        radius: width / 2
        color: activeColor
        opacity: 0
        visible: active

        SequentialAnimation on opacity {
            running: root.active
            loops: Animation.Infinite
            
            NumberAnimation {
                from: 0
                to: 0.3
                duration: 400
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                from: 0.3
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
            }
        }
    }

    // Label
    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 4
        text: isLeft ? "LEFT" : "RIGHT"
        font.pixelSize: 10
        font.weight: Font.Medium
        font.letterSpacing: 1
        color: active ? activeColor : "#666666"
        opacity: active ? 1.0 : 0.5

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
