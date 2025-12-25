// MediaPage.qml
// Media player page - Spotify-like UI

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    // Inline component for media buttons
    component MediaButton: Rectangle {
        property string icon: ""
        property bool primary: false
        property bool active: false

        signal clicked

        width: primary ? 64 : 48
        height: primary ? 64 : 48
        radius: width / 2
        color: primary ? "#4DC9CB" : (active ? "#4DC9CB40" : (mouseArea.containsMouse ? "#1F2830" : "#111821"))

        Text {
            anchors.centerIn: parent
            text: icon
            font.pixelSize: primary ? 28 : 20
            color: active ? "#4DC9CB" : "#F4F7FB"
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    // Inline component for playlist item
    component PlaylistItem: Rectangle {
        property string name: ""
        property string accentColor: "#1DB954"
        property int trackCount: 0
        property bool selected: false

        signal clicked

        width: parent.width
        height: 48
        radius: 8
        color: selected ? "#1F2830" : (playlistMouseArea.containsMouse ? "#111821" : "transparent")

        Row {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 12

            Rectangle {
                width: 32
                height: 32
                radius: 4
                color: accentColor
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: name === "Liked Songs" ? "\u2665" : "\u266B"
                    font.pixelSize: 14
                    color: "#F4F7FB"
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                Text {
                    text: name
                    font.pixelSize: 13
                    font.weight: selected ? Font.DemiBold : Font.Normal
                    color: selected ? "#4DC9CB" : "#F4F7FB"
                }

                Text {
                    text: trackCount + " tracks"
                    font.pixelSize: 11
                    color: "#7F95A6"
                }
            }
        }

        MouseArea {
            id: playlistMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    // Inline component for queue track item
    component QueueTrackItem: Rectangle {
        property int trackIndex: 0
        property string title: ""
        property string artist: ""
        property string artColor: "#333333"
        property bool isPlaying: false
        property int duration: 0

        signal clicked

        width: parent.width
        height: 56
        radius: 8
        color: isPlaying ? "#4DC9CB20" : (trackMouseArea.containsMouse ? "#111821" : "transparent")

        Row {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 12

            Text {
                text: trackIndex + 1
                font.pixelSize: 12
                color: isPlaying ? "#4DC9CB" : "#7F95A6"
                width: 24
                horizontalAlignment: Text.AlignRight
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                width: 40
                height: 40
                radius: 4
                color: artColor
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: isPlaying ? "\u25B6" : "\u266B"
                    font.pixelSize: isPlaying ? 12 : 16
                    color: "#F4F7FB"
                    opacity: 0.8
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2
                width: parent.width - 150

                Text {
                    text: title
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    color: isPlaying ? "#4DC9CB" : "#F4F7FB"
                    elide: Text.ElideRight
                    width: parent.width
                }

                Text {
                    text: artist
                    font.pixelSize: 11
                    color: "#7F95A6"
                    elide: Text.ElideRight
                    width: parent.width
                }
            }

            Text {
                text: formatTime(duration)
                font.pixelSize: 12
                color: "#7F95A6"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id: trackMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    function formatTime(ms) {
        var seconds = Math.floor(ms / 1000);
        var minutes = Math.floor(seconds / 60);
        seconds = seconds % 60;
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds;
    }

    Rectangle {
        anchors.fill: parent
        color: "#03090F"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Left panel - Playlists
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: "#111821"
            radius: 12

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text {
                    text: "Playlists"
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    color: "#F4F7FB"
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#1C2832"
                }

                ListView {
                    width: parent.width
                    height: parent.height - 40
                    clip: true
                    model: mediaController.playlists
                    spacing: 4

                    delegate: PlaylistItem {
                        width: ListView.view.width
                        name: modelData.name
                        accentColor: modelData.color
                        trackCount: modelData.trackCount
                        selected: mediaController.currentPlaylist === modelData.name
                        onClicked: mediaController.selectPlaylist(modelData.name)
                    }
                }
            }
        }

        // Center panel - Now Playing
        Rectangle {
            Layout.preferredWidth: 360
            Layout.fillHeight: true
            color: "#111821"
            radius: 12

            Flickable {
                anchors.fill: parent
                anchors.margins: 16
                contentWidth: width
                contentHeight: nowPlayingColumn.height
                clip: true

                Column {
                    id: nowPlayingColumn
                    width: parent.width
                    spacing: 20

                    // Album art
                    Rectangle {
                        width: 240
                        height: 240
                        radius: 12
                        color: mediaController.albumArtColor
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Gradient overlay
                        Rectangle {
                            anchors.fill: parent
                            radius: parent.radius
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.0
                                    color: "transparent"
                                }
                                GradientStop {
                                    position: 1.0
                                    color: "#00000040"
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "\u266B"
                            font.pixelSize: 72
                            color: "#FFFFFF"
                            opacity: 0.3
                        }
                    }

                    // Track info
                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 6
                        width: parent.width

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: mediaController.title
                            font.pixelSize: 20
                            font.weight: Font.DemiBold
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: mediaController.artist
                            font.pixelSize: 14
                            color: "#A0A0A0"
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: mediaController.album
                            font.pixelSize: 12
                            color: "#808080"
                        }
                    }

                    // Progress bar
                    Column {
                        width: parent.width
                        spacing: 6

                        Rectangle {
                            width: parent.width
                            height: 4
                            radius: 2
                            color: "#333333"

                            Rectangle {
                                width: parent.width * (mediaController.duration > 0 ? mediaController.position / mediaController.duration : 0)
                                height: parent.height
                                radius: 2
                                color: "#1DB954"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    var pos = (mouseX / width) * mediaController.duration;
                                    mediaController.seek(pos);
                                }
                            }
                        }

                        Row {
                            width: parent.width

                            Text {
                                text: formatTime(mediaController.position)
                                font.pixelSize: 11
                                color: "#808080"
                            }

                            Item {
                                width: parent.width - 70
                                height: 1
                            }

                            Text {
                                text: formatTime(mediaController.duration)
                                font.pixelSize: 11
                                color: "#808080"
                            }
                        }
                    }

                    // Playback controls
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 16

                        MediaButton {
                            icon: "\u2728"  // Shuffle
                            active: mediaController.shuffleMode
                            onClicked: mediaController.setShuffleMode(!mediaController.shuffleMode)
                        }

                        MediaButton {
                            icon: "\u23EE"
                            onClicked: mediaController.previous()
                        }

                        MediaButton {
                            icon: mediaController.playing ? "\u23F8" : "\u25B6"
                            primary: true
                            onClicked: mediaController.togglePlayPause()
                        }

                        MediaButton {
                            icon: "\u23ED"
                            onClicked: mediaController.next()
                        }

                        MediaButton {
                            icon: mediaController.repeatMode === 2 ? "\u21BB\u00B9" : "\u21BB"
                            active: mediaController.repeatMode > 0
                            onClicked: mediaController.cycleRepeatMode()
                        }
                    }

                    // Secondary controls
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 24

                        // Like button
                        Rectangle {
                            width: 36
                            height: 36
                            radius: 18
                            color: likeMouseArea.containsMouse ? "#333333" : "transparent"

                            Text {
                                anchors.centerIn: parent
                                text: mediaController.liked ? "\u2665" : "\u2661"
                                font.pixelSize: 18
                                color: mediaController.liked ? "#4DC9CB" : "#F4F7FB"
                            }

                            MouseArea {
                                id: likeMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: mediaController.toggleLiked()
                            }
                        }
                    }

                    // Volume control
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 12

                        Text {
                            text: "\uD83D\uDD0A"
                            font.pixelSize: 14
                            color: "#808080"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Rectangle {
                            width: 150
                            height: 4
                            radius: 2
                            color: "#333333"
                            anchors.verticalCenter: parent.verticalCenter

                            Rectangle {
                                width: parent.width * (mediaController.volume / 100)
                                height: parent.height
                                radius: 2
                                color: "#4DC9CB"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    var vol = Math.round((mouseX / width) * 100);
                                    mediaController.setVolume(vol);
                                }
                            }
                        }

                        Text {
                            text: mediaController.volume + "%"
                            font.pixelSize: 11
                            color: "#808080"
                            width: 30
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }

        // Right panel - Queue
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#111821"
            radius: 12

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Row {
                    width: parent.width
                    spacing: 8

                    Text {
                        text: mediaController.currentPlaylist
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        color: "#1DB954"
                    }

                    Text {
                        text: "- " + mediaController.queue.length + " tracks"
                        font.pixelSize: 14
                        color: "#808080"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#333333"
                }

                ListView {
                    width: parent.width
                    height: parent.height - 40
                    clip: true
                    model: mediaController.queue
                    spacing: 4

                    delegate: QueueTrackItem {
                        width: ListView.view.width
                        trackIndex: index
                        title: modelData.title
                        artist: modelData.artist
                        artColor: modelData.artColor
                        duration: modelData.duration
                        isPlaying: index === mediaController.currentTrackIndex
                        onClicked: mediaController.playTrackAt(index)
                    }
                }
            }
        }
    }
}
