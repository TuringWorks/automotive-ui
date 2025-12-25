// ProjectionPage.qml
// Apple CarPlay / Android Auto integration page

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root

    // Basic toggle switch
    component ToggleSwitch: Rectangle {
        property bool checked: false
        property color accentColor: "#60A5FA"
        signal toggled(bool value)

        width: 44
        height: 24
        radius: 12
        color: checked ? accentColor : "#333333"

        Rectangle {
            width: 20
            height: 20
            radius: 10
            color: "#FFFFFF"
            anchors.verticalCenter: parent.verticalCenter
            x: checked ? parent.width - width - 2 : 2

            Behavior on x {
                NumberAnimation {
                    duration: 140
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: toggled(!checked)
        }
    }

    // Action button with hover feedback
    component ActionButton: Rectangle {
        property string label: ""
        property bool primary: false
        property bool danger: false
        property color accentColor: "#60A5FA"
        property bool enabled: true
        property bool hovering: false
        signal clicked

        width: 132
        height: 44
        radius: 12
        opacity: enabled ? 1.0 : 0.4
        border.width: (primary || danger) ? 0 : 1
        border.color: "#333333"
        color: danger ? (hovering && enabled ? "#F87171" : "#EF4444") : (primary ? (hovering && enabled ? Qt.lighter(accentColor, 1.1) : accentColor) : (hovering && enabled ? "#333333" : "#262626"))

        Text {
            anchors.centerIn: parent
            text: label
            font.pixelSize: 14
            font.weight: Font.Medium
            color: danger ? "#FFFFFF" : (primary ? "#000000" : "#FFFFFF")
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            enabled: parent.enabled
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onEntered: parent.hovering = true
            onExited: parent.hovering = false
            onCanceled: parent.hovering = false
            onClicked: parent.clicked()
        }
    }

    // Card to show projection state for a platform
    component ProjectionCard: Rectangle {
        property string title: ""
        property string icon: ""
        property string statusText: ""
        property string deviceName: ""
        property bool connected: false
        property bool wirelessEnabled: false
        property color accentColor: "#60A5FA"

        signal connectRequested
        signal disconnectRequested
        signal wirelessToggled(bool enabled)

        Layout.fillWidth: true
        Layout.preferredHeight: 280
        radius: 16
        color: "#111821"
        border.width: 1
        border.color: connected ? accentColor + "80" : "#262626"

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            Row {
                spacing: 12

                Rectangle {
                    width: 48
                    height: 48
                    radius: 16
                    color: accentColor + "20"

                    Text {
                        anchors.centerIn: parent
                        text: icon
                        font.pixelSize: 24
                    }
                }

                Column {
                    spacing: 6

                    Text {
                        text: title
                        font.pixelSize: 20
                        font.weight: Font.DemiBold
                        color: "#FFFFFF"
                    }

                    Text {
                        text: connected ? ("Streaming from " + deviceName) : ("Ready for " + deviceName)
                        font.pixelSize: 14
                        color: "#A0A0A0"
                    }
                }
            }

            Text {
                text: statusText
                font.pixelSize: 14
                color: connected ? accentColor : "#D4D4D4"
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "#2F2F2F"
            }

            RowLayout {
                width: parent.width
                spacing: 24

                Column {
                    spacing: 4
                    Layout.fillWidth: true

                    Text {
                        text: "Wireless preference"
                        font.pixelSize: 12
                        color: "#808080"
                    }

                    Row {
                        spacing: 8

                        ToggleSwitch {
                            checked: wirelessEnabled
                            accentColor: accentColor
                            onToggled: wirelessToggled(value)
                        }

                        Text {
                            text: wirelessEnabled ? "Wireless" : "USB only"
                            font.pixelSize: 14
                            color: "#FFFFFF"
                        }
                    }
                }

                Column {
                    spacing: 8
                    Layout.alignment: Qt.AlignTop

                    ActionButton {
                        label: connected ? "Reconnect" : "Connect"
                        primary: true
                        accentColor: accentColor
                        onClicked: connectRequested()
                    }

                    ActionButton {
                        label: "Disconnect"
                        danger: connected
                        enabled: connected
                        onClicked: disconnectRequested()
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#03090F"
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 24
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 24

            Text {
                text: "Phone Apps"
                font.pixelSize: 28
                font.weight: Font.DemiBold
                color: "#FFFFFF"
            }

            Text {
                text: "Manage trusted devices, preferred connection modes, and session status for Apple CarPlay and Android Auto."
                font.pixelSize: 14
                color: "#A0A0A0"
                wrapMode: Text.WordWrap
            }

            Rectangle {
                id: sessionCard
                Layout.fillWidth: true
                radius: 20
                color: "#111827"
                border.color: projectionController.sessionActive ? (projectionController.sessionPlatform === "Android Auto" ? "#34D39940" : "#60A5FA40") : "#1F2937"
                property color accent: projectionController.sessionPlatform === "Android Auto" ? "#34D399" : "#60A5FA"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: projectionController.sessionActive ? projectionController.sessionPlatform + " session" : "No active session"
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                            color: "#FFFFFF"
                        }

                        Rectangle {
                            width: 10
                            height: 10
                            radius: 5
                            color: projectionController.sessionActive ? sessionCard.accent : "#4B5563"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    Text {
                        text: projectionController.sessionSummary
                        font.pixelSize: 14
                        color: "#94A3B8"
                        wrapMode: Text.WordWrap
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 24

                ProjectionCard {
                    icon: ""
                    title: "Apple CarPlay"
                    accentColor: "#60A5FA"
                    deviceName: projectionController.carPlayDeviceName
                    statusText: projectionController.carPlayStatus
                    connected: projectionController.carPlayConnected
                    wirelessEnabled: projectionController.carPlayWirelessEnabled
                    onConnectRequested: projectionController.connectCarPlay()
                    onDisconnectRequested: projectionController.disconnectCarPlay()
                    onWirelessToggled: projectionController.setCarPlayWirelessEnabled(enabled)
                }

                ProjectionCard {
                    icon: "🤖"
                    title: "Android Auto"
                    accentColor: "#34D399"
                    deviceName: projectionController.androidAutoDeviceName
                    statusText: projectionController.androidAutoStatus
                    connected: projectionController.androidAutoConnected
                    wirelessEnabled: projectionController.androidAutoWirelessEnabled
                    onConnectRequested: projectionController.connectAndroidAuto()
                    onDisconnectRequested: projectionController.disconnectAndroidAuto()
                    onWirelessToggled: projectionController.setAndroidAutoWirelessEnabled(enabled)
                }
            }

            Rectangle {
                visible: projectionController.sessionActive
                Layout.fillWidth: true
                Layout.preferredHeight: visible ? 360 : 0
                Layout.minimumHeight: visible ? 360 : 0
                radius: 24
                color: projectionController.sessionPlatform === "Android Auto" ? "#041913" : "#030A1C"
                border.color: (projectionController.sessionPlatform === "Android Auto" ? "#34D39940" : "#60A5FA40")
                property var navTile: projectionController.sessionNavTile
                property var mediaTile: projectionController.sessionMediaTile

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 220
                            radius: 20
                            color: "#0B1430"
                            border.color: (projectionController.sessionPlatform === "Android Auto" ? "#34D39940" : "#60A5FA40")

                            Column {
                                anchors.fill: parent
                                anchors.margins: 18
                                spacing: 10
                                property var tile: projectionPreview.navTile

                                Text {
                                    text: tile.title || "Navigation"
                                    font.pixelSize: 18
                                    font.weight: Font.DemiBold
                                    color: "#FFFFFF"
                                }

                                Text {
                                    text: tile.instruction || "Start guidance"
                                    font.pixelSize: 22
                                    font.weight: Font.Medium
                                    color: projectionPreview.border.color
                                    wrapMode: Text.WordWrap
                                }

                                Text {
                                    text: tile.detail || ""
                                    font.pixelSize: 13
                                    color: "#9CA3AF"
                                    wrapMode: Text.WordWrap
                                }

                                Rectangle {
                                    width: parent.width
                                    height: 1
                                    color: "#1F2937"
                                }

                                Row {
                                    spacing: 8

                                    Rectangle {
                                        width: 46
                                        height: 46
                                        radius: 14
                                        color: projectionPreview.border.color

                                        Text {
                                            anchors.centerIn: parent
                                            text: tile.icon || "➤"
                                            font.pixelSize: 24
                                        }
                                    }

                                    Column {
                                        spacing: 2
                                        width: parent.width - 70

                                        Text {
                                            text: tile.eta || "--"
                                            font.pixelSize: 16
                                            font.weight: Font.Medium
                                            color: "#FFFFFF"
                                        }

                                        Text {
                                            text: projectionController.sessionPlatform === "Android Auto" ? "Assistant listening" : "Siri available"
                                            font.pixelSize: 12
                                            color: "#9CA3AF"
                                        }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 220
                            radius: 20
                            color: "#120B1F"
                            border.color: (projectionController.sessionPlatform === "Android Auto" ? "#34D39940" : "#60A5FA40")

                            Column {
                                anchors.fill: parent
                                anchors.margins: 18
                                spacing: 12
                                property var tile: projectionPreview.mediaTile

                                Row {
                                    spacing: 12

                                    Rectangle {
                                        width: 64
                                        height: 64
                                        radius: 20
                                        color: tile.accent ? tile.accent + "30" : "#1F2937"

                                        Text {
                                            anchors.centerIn: parent
                                            text: tile.artwork || "♪"
                                            font.pixelSize: 28
                                        }
                                    }

                                    Column {
                                        spacing: 4

                                        Text {
                                            text: tile.title || "Now playing"
                                            font.pixelSize: 18
                                            font.weight: Font.DemiBold
                                            color: "#FFFFFF"
                                        }

                                        Text {
                                            text: tile.artist || "Media source"
                                            font.pixelSize: 14
                                            color: "#9CA3AF"
                                        }

                                        Text {
                                            text: tile.album || ""
                                            font.pixelSize: 12
                                            color: "#6B7280"
                                        }
                                    }
                                }

                                Rectangle {
                                    width: parent.width
                                    height: 6
                                    radius: 3
                                    color: "#1F2937"

                                    Rectangle {
                                        width: parent.width * 0.45
                                        height: parent.height
                                        radius: 3
                                        color: tile.accent || projectionPreview.border.color
                                    }
                                }

                                Row {
                                    spacing: 16
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Rectangle {
                                        width: 44
                                        height: 44
                                        radius: 22
                                        color: "#1F2937"

                                        Text {
                                            anchors.centerIn: parent
                                            text: "⏮"
                                            font.pixelSize: 18
                                        }
                                    }

                                    Rectangle {
                                        width: 64
                                        height: 64
                                        radius: 32
                                        color: tile.accent || projectionPreview.border.color

                                        Text {
                                            anchors.centerIn: parent
                                            text: "⏯"
                                            font.pixelSize: 26
                                            color: "#000000"
                                        }
                                    }

                                    Rectangle {
                                        width: 44
                                        height: 44
                                        radius: 22
                                        color: "#1F2937"

                                        Text {
                                            anchors.centerIn: parent
                                            text: "⏭"
                                            font.pixelSize: 18
                                        }
                                    }
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Repeater {
                            model: projectionController.sessionApps

                            Rectangle {
                                visible: index < 6
                                Layout.preferredWidth: visible ? 86 : 0
                                Layout.preferredHeight: visible ? 96 : 0
                                radius: 22
                                color: modelData.accent ? modelData.accent + "25" : "#1F2937"

                                Column {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 6

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: modelData.icon
                                        font.pixelSize: 20
                                    }

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: modelData.name
                                        font.pixelSize: 12
                                        font.weight: Font.Medium
                                        color: "#FFFFFF"
                                        wrapMode: Text.WordWrap
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Text {
                visible: !projectionController.sessionActive
                text: "Connect a CarPlay or Android Auto device to preview the projection UI."
                font.pixelSize: 14
                color: "#6B7280"
                wrapMode: Text.WordWrap
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: guidanceColumn.height + 32
                radius: 16
                color: "#1A1A1A"

                Column {
                    id: guidanceColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 20
                    spacing: 12

                    Text {
                        text: "Driver distraction safeguards"
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        color: "#FFFFFF"
                    }

                    Repeater {
                        model: ["Projection is disabled while vehicle is in valet mode.", "Only trusted devices from Secure Settings appear here.", "USB connections are monitored for rapid disconnect detection."]

                        Row {
                            spacing: 8
                            width: guidanceColumn.width

                            Text {
                                text: "•"
                                font.pixelSize: 14
                                color: "#9CA3AF"
                            }

                            Text {
                                text: modelData
                                font.pixelSize: 14
                                color: "#9CA3AF"
                                wrapMode: Text.WordWrap
                                width: guidanceColumn.width - 32
                            }
                        }
                    }
                }
            }
        }
    }
}
