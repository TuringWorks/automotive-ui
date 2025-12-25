// PhonePage.qml
// Refined communication hub with connection + dialer

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    property string dialDigits: ""
    property int phoneTab: 0

    function appendDigit(d) {
        if (dialDigits.length < 15)
            dialDigits += d;
    }

    function backspace() {
        if (dialDigits.length > 0)
            dialDigits = dialDigits.slice(0, dialDigits.length - 1);
    }

    Rectangle {
        anchors.fill: parent
        color: "#05090F"
    }

    component QuickAction: Rectangle {
        property string label: ""
        property string icon: ""
        signal triggered

        width: 110
        height: 44
        radius: 16
        color: "#121D25"
        border.color: "#1F2B36"
        Row {
            anchors.centerIn: parent
            spacing: 6
            Text {
                text: icon
                font.pixelSize: 14
                color: "#E6F2FF"
            }
            Text {
                text: label
                font.pixelSize: 12
                font.weight: Font.Medium
                color: "#8FA5BD"
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: triggered()
        }
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 24
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 20

            Text {
                text: "Phone"
                font.pixelSize: 28
                font.weight: Font.DemiBold
                color: "#F4F8FC"
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                radius: 28
                color: "#111C27"
                border.color: "#1F2A36"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 20

                    Column {
                        Layout.fillWidth: true
                        spacing: 6

                        Text {
                            text: phoneController.connected ? phoneController.phoneName : "No device connected"
                            font.pixelSize: 20
                            font.weight: Font.DemiBold
                            color: "#E6F0FF"
                        }

                        Text {
                            text: phoneController.connected ? "Bluetooth link secure" : "Please pair a phone"
                            font.pixelSize: 14
                            color: "#8FA3B8"
                        }

                        Text {
                            visible: phoneController.inCall
                            text: "In call · " + phoneController.callerName + " (" + phoneController.callerNumber + ")"
                            font.pixelSize: 13
                            color: "#74DBB4"
                        }
                    }

                    Row {
                        spacing: 12

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 20
                            color: phoneController.inCall ? "#F87171" : "#4ADE80"
                            Text {
                                anchors.centerIn: parent
                                text: phoneController.inCall ? "End" : "Call"
                                color: "#051312"
                                font.pixelSize: 14
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (phoneController.inCall)
                                        phoneController.hangUp();
                                    else if (dialDigits.length > 0)
                                        phoneController.dial(dialDigits);
                                }
                            }
                        }

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 20
                            color: "#1F2D3A"
                            Text {
                                anchors.centerIn: parent
                                text: phoneController.muted ? "Unmute" : "Mute"
                                color: "#A6B8C6"
                                font.pixelSize: 12
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: phoneController.mute(!phoneController.muted)
                            }
                        }
                    }
                }
            }

            Row {
                spacing: 12
                QuickAction {
                    label: phoneController.muted ? "Muted" : "Mute"
                    icon: phoneController.muted ? "🔇" : "🔊"
                    onTriggered: phoneController.mute(!phoneController.muted)
                }
                QuickAction {
                    label: "Wi-Fi"
                    icon: "📶"
                }
                QuickAction {
                    label: "History"
                    icon: "🕓"
                    onTriggered: phoneTab = 1
                }
                QuickAction {
                    label: "Pair"
                    icon: "🔗"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 130
                radius: 24
                color: "#0F1822"
                border.color: "#1B2733"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12

                    Repeater {
                        model: Math.min(phoneController.contacts.length, 4)

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            radius: 18
                            color: "#182533"
                            border.color: "#223343"

                            Column {
                                anchors.centerIn: parent
                                spacing: 6
                                property var entry: phoneController.contacts[index]
                                Text {
                                    text: entry ? entry.name : ""
                                    font.pixelSize: 14
                                    color: "#F2F7FF"
                                }
                                Text {
                                    text: entry ? entry.number : ""
                                    font.pixelSize: 12
                                    color: "#8EA3B8"
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    var entry = phoneController.contacts[index];
                                    if (entry)
                                        phoneController.dial(entry.number);
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 500
                spacing: 20

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 500
                    radius: 24
                    color: "#101820"
                    border.color: "#1D2731"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 12

                        Row {
                            spacing: 12
                            Rectangle {
                                width: 110
                                height: 38
                                radius: 14
                                color: phoneTab === 0 ? "#4DC9CB" : "#17232F"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Contacts"
                                    color: phoneTab === 0 ? "#052027" : "#9EB4C9"
                                    font.pixelSize: 12
                                    font.weight: Font.Medium
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: phoneTab = 0
                                }
                            }
                            Rectangle {
                                width: 110
                                height: 38
                                radius: 14
                                color: phoneTab === 1 ? "#4DC9CB" : "#17232F"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Recents"
                                    color: phoneTab === 1 ? "#052027" : "#9EB4C9"
                                    font.pixelSize: 12
                                    font.weight: Font.Medium
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: phoneTab = 1
                                }
                            }
                        }

                        StackLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            currentIndex: phoneTab

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                ListView {
                                    anchors.fill: parent
                                    clip: true
                                    spacing: 6
                                    model: phoneController.contacts
                                    delegate: Rectangle {
                                        width: ListView.view.width
                                        height: 56
                                        radius: 14
                                        color: mouse.containsMouse ? "#1F2B37" : "transparent"

                                        Row {
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            spacing: 12

                                            Rectangle {
                                                width: 36
                                                height: 36
                                                radius: 12
                                                color: "#18232F"

                                                Text {
                                                    anchors.centerIn: parent
                                                    text: modelData.name.charAt(0)
                                                    color: "#E8F0FA"
                                                    font.pixelSize: 16
                                                }
                                            }

                                            Column {
                                                spacing: 2
                                                Text {
                                                    text: modelData.name
                                                    font.pixelSize: 14
                                                    color: "#F5F8FB"
                                                }
                                                Text {
                                                    text: modelData.number
                                                    font.pixelSize: 12
                                                    color: "#8FA3B8"
                                                }
                                            }

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            Rectangle {
                                                width: 90
                                                height: 34
                                                radius: 12
                                                color: "#4DC9CB"

                                                Text {
                                                    anchors.centerIn: parent
                                                    text: "Call"
                                                    color: "#052026"
                                                    font.pixelSize: 12
                                                }

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: phoneController.dial(modelData.number)
                                                }
                                            }
                                        }

                                        MouseArea {
                                            id: mouse
                                            anchors.fill: parent
                                            hoverEnabled: true
                                        }
                                    }
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                ListView {
                                    anchors.fill: parent
                                    clip: true
                                    spacing: 6
                                    model: phoneController.recentCalls
                                    delegate: Rectangle {
                                        width: ListView.view.width
                                        height: 48
                                        radius: 12
                                        color: "#0F151C"

                                        Row {
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            spacing: 8
                                            Text {
                                                text: modelData.name
                                                font.pixelSize: 13
                                                color: "#E4EDF7"
                                            }
                                            Text {
                                                text: modelData.number
                                                font.pixelSize: 12
                                                color: "#8FA3B8"
                                            }
                                            Item {
                                                Layout.fillWidth: true
                                            }
                                            Text {
                                                text: modelData.time
                                                font.pixelSize: 12
                                                color: "#6B7D8F"
                                            }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: phoneController.dial(modelData.number)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 360
                    Layout.preferredHeight: 500
                    radius: 24
                    color: "#111C27"
                    border.color: "#1D2833"

                    Column {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 16

                        Text {
                            text: "Keypad"
                            font.pixelSize: 16
                            font.weight: Font.DemiBold
                            color: "#EEF5FB"
                        }

                        Rectangle {
                            width: parent.width
                            height: 48
                            radius: 16
                            color: "#0C141D"
                            border.color: "#1E2B36"
                            Row {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 10
                                Text {
                                    text: "📞"
                                    font.pixelSize: 16
                                    color: "#7F95A6"
                                }
                                Text {
                                    text: dialDigits.length ? dialDigits : "Enter number"
                                    font.pixelSize: 16
                                    color: dialDigits.length ? "#F6FBFF" : "#6E8398"
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                                MouseArea {
                                    width: 24
                                    height: 24
                                    anchors.verticalCenter: parent.verticalCenter
                                    onClicked: backspace()
                                    Text {
                                        anchors.centerIn: parent
                                        text: "⌫"
                                        color: "#879BAD"
                                    }
                                }
                            }
                        }

                        GridLayout {
                            columns: 3
                            columnSpacing: 14
                            rowSpacing: 14

                            Repeater {
                                model: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"]

                                Rectangle {
                                    width: 90
                                    height: 70
                                    radius: 18
                                    color: "#182534"
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData
                                        font.pixelSize: 22
                                        color: "#F4F8FC"
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: appendDigit(modelData)
                                    }
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            anchors.horizontalCenter: parent.horizontalCenter

                            Rectangle {
                                width: 70
                                height: 70
                                radius: 24
                                color: "#4ADE80"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Dial"
                                    color: "#03160B"
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: if (dialDigits.length)
                                        phoneController.dial(dialDigits)
                                }
                            }

                            Rectangle {
                                width: 70
                                height: 70
                                radius: 24
                                color: phoneController.inCall ? "#F87171" : "#1F2B37"
                                Text {
                                    anchors.centerIn: parent
                                    text: phoneController.inCall ? "Hang" : "Clear"
                                    color: "#F4F8FC"
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (phoneController.inCall)
                                            phoneController.hangUp();
                                        else
                                            dialDigits = "";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
