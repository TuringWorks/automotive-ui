// WhyUnavailable.qml
// "Why Unavailable" Explanation Component
// Part of: Infotainment Autonomy Center
// Spec: SOTIF-UI-001, FR-INF-ADAS-002

import QtQuick
import QtQuick.Layouts
import DesignSystem 1.0

/**
 * Why Unavailable - Explains current system limitations.
 *
 * Provides explicit "why unavailable" explanations:
 * - Lane lines not detected
 * - Weather conditions
 * - Road type not supported
 * - Sensor issues
 *
 * SOTIF: Avoid overstating capability.
 */
Rectangle {
    id: root

    property var limitations: []

    radius: 12
    color: Colors.warningBackground
    border.color: Colors.warning
    border.width: 1
    implicitHeight: contentColumn.height + Spacing.medium * 2

    ColumnLayout {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Spacing.medium
        spacing: Spacing.small

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.small

            Rectangle {
                width: 24
                height: 24
                radius: 12
                color: Colors.warning

                Text {
                    anchors.centerIn: parent
                    text: "!"
                    font: Typography.labelMedium
                    color: Colors.textOnPrimary
                }
            }

            Text {
                text: "System Limitations"
                font: Typography.headingSmall
                color: Colors.warning
            }

            Item { Layout.fillWidth: true }

            Text {
                text: root.limitations.length + " active"
                font: Typography.caption
                color: Colors.textSecondary
            }
        }

        // Limitation list
        Repeater {
            model: root.limitations

            Rectangle {
                Layout.fillWidth: true
                height: limitationRow.height + Spacing.small * 2
                radius: 4
                color: Colors.surface

                RowLayout {
                    id: limitationRow
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: Spacing.small
                    spacing: Spacing.small

                    // Icon based on limitation type
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 4
                        color: Colors.surfaceTertiary

                        Text {
                            anchors.centerIn: parent
                            text: getLimitationIcon(modelData)
                            font.pixelSize: 16
                        }
                    }

                    // Limitation text
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: getLimitationTitle(modelData)
                            font: Typography.bodySmall
                            color: Colors.textPrimary
                        }

                        Text {
                            text: getLimitationDescription(modelData)
                            font: Typography.caption
                            color: Colors.textSecondary
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }

        // Help text
        Text {
            Layout.fillWidth: true
            Layout.topMargin: Spacing.xsmall
            text: "These limitations may change as conditions improve."
            font: Typography.caption
            color: Colors.textTertiary
            horizontalAlignment: Text.AlignCenter
        }
    }

    // Helper functions
    function getLimitationIcon(limitation) {
        // Parse limitation string for type
        const lower = limitation.toLowerCase()
        if (lower.includes("lane")) return "🛣️"
        if (lower.includes("weather") || lower.includes("rain") || lower.includes("snow")) return "🌧️"
        if (lower.includes("road")) return "⚠️"
        if (lower.includes("sensor") || lower.includes("camera") || lower.includes("radar")) return "📡"
        if (lower.includes("speed")) return "🚗"
        if (lower.includes("gps") || lower.includes("map")) return "📍"
        return "ℹ️"
    }

    function getLimitationTitle(limitation) {
        // Extract or generate a title
        const lower = limitation.toLowerCase()
        if (lower.includes("lane")) return "Lane Detection Limited"
        if (lower.includes("weather")) return "Weather Conditions"
        if (lower.includes("road type")) return "Road Type Not Supported"
        if (lower.includes("sensor")) return "Sensor Issue"
        if (lower.includes("speed")) return "Speed Limit"
        if (lower.includes("gps")) return "GPS/Map Data"
        return limitation.substring(0, 30)
    }

    function getLimitationDescription(limitation) {
        return limitation
    }
}
