// NavigationBar.qml
// Top navigation bar for infotainment

import QtQuick 2.15
import "../DesignSystem"

Rectangle {
    id: root

    property string title: ""
    property bool showBack: false
    property alias leftContent: leftLoader.sourceComponent
    property alias rightContent: rightLoader.sourceComponent

    signal backClicked()

    height: Spacing.navBarHeight
    color: Colors.surface

    // Back button
    IconButton {
        id: backButton
        anchors.left: parent.left
        anchors.leftMargin: Spacing.md
        anchors.verticalCenter: parent.verticalCenter
        iconSource: "qrc:/assets/icons/arrow-left.svg"
        visible: root.showBack
        showBackground: false
        onClicked: root.backClicked()
    }

    // Left custom content
    Loader {
        id: leftLoader
        anchors.left: backButton.visible ? backButton.right : parent.left
        anchors.leftMargin: Spacing.md
        anchors.verticalCenter: parent.verticalCenter
    }

    // Title
    Text {
        anchors.centerIn: parent
        text: root.title
        font.pixelSize: Typography.sizeXl
        font.weight: Font.SemiBold
        color: Colors.textPrimary
    }

    // Right custom content
    Loader {
        id: rightLoader
        anchors.right: parent.right
        anchors.rightMargin: Spacing.md
        anchors.verticalCenter: parent.verticalCenter
    }

    // Bottom border
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Colors.border
    }
}
