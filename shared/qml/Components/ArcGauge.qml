// ArcGauge.qml
// Compact arc gauge for secondary values (battery, power, etc.)
// Safety: Display-only

import QtQuick 2.15
import "../DesignSystem"

Item {
    id: root

    property real value: 0
    property real minValue: 0
    property real maxValue: 100
    property string label: ""
    property string unit: ""
    property bool valid: true
    property bool stale: false
    property bool showValue: true

    // Visual customization
    property real startAngle: -120
    property real endAngle: 120
    property color activeColor: Colors.clusterGaugeActive

    width: Tokens.gaugeMedium
    height: Tokens.gaugeMedium

    GaugeBase {
        id: gauge
        anchors.fill: parent
        value: root.value
        minValue: root.minValue
        maxValue: root.maxValue
        valid: root.valid
        stale: root.stale
        startAngle: root.startAngle
        endAngle: root.endAngle
        trackWidth: 8
        arcWidth: 8
        activeColor: root.activeColor
    }

    Column {
        anchors.centerIn: parent
        spacing: Spacing.xxs
        visible: root.showValue

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.valid ? Math.round(root.value).toString() : "—"
            font.pixelSize: Typography.size2xl
            font.weight: Font.Medium
            color: root.valid ? Colors.textPrimary : Colors.textTertiary
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.unit
            font.pixelSize: Typography.sizeSm
            color: Colors.textSecondary
            visible: root.unit !== ""
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Spacing.md
        text: root.label
        font.pixelSize: Typography.sizeSm
        font.weight: Font.Medium
        color: Colors.textSecondary
        visible: root.label !== ""
    }
}
