// NowPlayingBar.qml
// Persistent now playing bar with premium design

import QtQuick 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    // State management
    property bool isMinimized: false

    // Hide when no music is playing
    visible: mediaController.title !== "" && mediaController.title !== "No track"
    height: visible ? (isMinimized ? 60 : 100) : 0
    opacity: visible ? 1.0 : 0.0

    color: "#090F16"
    radius: 32
    anchors.margins: 16
    border.color: "#16222C"
    border.width: 1
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: -8
        radius: 32
        samples: 25
        color: "#00000080"
    }

    // Smooth transitions
    Behavior on height {
        NumberAnimation {
            duration: 250
            easing.type: Easing.OutCubic
        }
    }

    Behavior on opacity {
        NumberAnimation {
            duration: 250
            easing.type: Easing.OutCubic
        }
    }

    // Inline component for mini buttons with enhanced animations
    component MiniButton: Rectangle {
        property string icon: ""
        property bool primary: false
        signal clicked

        width: 44
        height: 44
        radius: 22
        color: primary ? "#60A5FA" : (mouseArea.containsMouse ? "#1F2830" : "transparent")

        // Smooth color transition
        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        // Scale animation on hover
        scale: mouseArea.containsMouse ? 1.08 : 1.0
        Behavior on scale {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        // Glow effect for primary button
        layer.enabled: primary
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 0
            radius: 12
            samples: 17
            color: "#60A5FA50"
        }

        Text {
            anchors.centerIn: parent
            text: icon
            font.pixelSize: 18
            color: primary ? "#FFFFFF" : "#E8F3FB"

            // Subtle scale on hover
            scale: mouseArea.containsMouse ? 1.1 : 1.0
            Behavior on scale {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                // Click feedback
                parent.scale = 0.9;
                clickTimer.start();
                parent.clicked();
            }

            Timer {
                id: clickTimer
                interval: 100
                onTriggered: parent.scale = 1.0
            }
        }
    }

    // Enhanced progress bar
    Rectangle {
        id: progressBarBackground
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 5
        radius: 2.5
        color: "#14222D"

        Rectangle {
            id: progressBar
            width: parent.width * (mediaController.duration > 0 ? mediaController.position / mediaController.duration : 0)
            height: parent.height
            radius: parent.radius
            color: "#4DC9CB"

            // Smooth width animation
            Behavior on width {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.Linear
                }
            }

            // Glow effect
            layer.enabled: true
            layer.effect: DropShadow {
                horizontalOffset: 0
                verticalOffset: 0
                radius: 8
                samples: 17
                color: "#4DC9CB40"
            }
        }

        // Interactive progress bar
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                var pos = (mouseX / width) * mediaController.duration;
                mediaController.seek(pos);
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        // Album art with improved styling
        Rectangle {
            id: albumArt
            width: 68
            height: 68
            radius: 20
            color: mediaController.albumArtColor || "#27333E"
            Layout.alignment: Qt.AlignVCenter

            // Subtle glow
            layer.enabled: true
            layer.effect: DropShadow {
                horizontalOffset: 0
                verticalOffset: 0
                radius: 12
                samples: 17
                color: Qt.rgba(0, 0, 0, 0.4)
            }

            Text {
                anchors.centerIn: parent
                text: "♫"
                font.pixelSize: 28
                color: "#FFFFFF"
                opacity: 0.5
            }
        }

        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 7

            Text {
                text: mediaController.title || "No track playing"
                font.pixelSize: 18
                font.weight: Font.DemiBold
                color: "#F4F9FF"
                elide: Text.ElideRight
                width: parent.width
                font.letterSpacing: 0.2
            }

            Row {
                spacing: 8

                Text {
                    text: mediaController.artist
                    font.pixelSize: 13
                    color: "#8CA0AF"
                    elide: Text.ElideRight
                }

                Text {
                    text: mediaController.artist && mediaController.album ? "•" : ""
                    font.pixelSize: 13
                    color: "#6F8797"
                }

                Text {
                    text: mediaController.album || ""
                    font.pixelSize: 13
                    color: "#7F95A6"
                    elide: Text.ElideRight
                }
            }
        }

        Row {
            Layout.alignment: Qt.AlignVCenter
            spacing: 16

            MiniButton {
                icon: "⏮"
                onClicked: mediaController.previous()
            }

            MiniButton {
                icon: mediaController.playing ? "⏸" : "▶"
                primary: true
                onClicked: mediaController.togglePlayPause()
            }

            MiniButton {
                icon: "⏭"
                onClicked: mediaController.next()
            }
        }

        // Time display
        Text {
            text: formatTime(mediaController.position) + " / " + formatTime(mediaController.duration)
            font.pixelSize: 12
            color: "#7F95A6"
            Layout.alignment: Qt.AlignVCenter
            font.family: "monospace"
        }
    }

    // Click to expand with visual feedback
    MouseArea {
        anchors.fill: parent
        z: -1
        hoverEnabled: true

        onClicked: appFramework.navigateTo(1)  // Go to media page

        // Subtle hover effect on entire bar
        onEntered: root.border.color = "#1F3442"
        onExited: root.border.color = "#16222C"
    }

    // Smooth border color transition
    Behavior on border.color {
        ColorAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    // Helper function for time formatting
    function formatTime(ms) {
        var seconds = Math.floor(ms / 1000);
        var minutes = Math.floor(seconds / 60);
        seconds = seconds % 60;
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds;
    }
}
