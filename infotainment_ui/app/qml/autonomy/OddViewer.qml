// OddViewer.qml
// Operational Design Domain (ODD) Viewer
// Part of: Infotainment Autonomy Center
// Spec: FR-INF-ADAS-002

import QtQuick
import QtQuick.Layouts
import DesignSystem 1.0

/**
 * ODD Viewer - Displays Operational Design Domain boundaries.
 *
 * Shows:
 * - Map with ODD coverage
 * - Current ODD status
 * - Boundary constraints (road type, weather, speed)
 * - Route ODD compatibility
 */
Rectangle {
    id: root

    // ODD data
    property int oddStatus: 1  // 0=Unknown, 1=Inside, 2=Approaching, 3=Outside, 4=Degraded
    property string roadType: "Highway"
    property double maxSpeedKph: 130
    property double minSpeedKph: 30
    property string weatherLimit: "Light Rain"
    property var oddReasons: []

    color: Colors.surfaceSecondary

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.large
        spacing: Spacing.medium

        // Header
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "Operational Design Domain"
                font: Typography.headingMedium
                color: Colors.textPrimary
            }

            Item { Layout.fillWidth: true }

            // ODD Status badge
            Rectangle {
                width: statusText.width + Spacing.medium * 2
                height: 32
                radius: 16
                color: getStatusColor()

                Text {
                    id: statusText
                    anchors.centerIn: parent
                    text: getStatusText()
                    font: Typography.labelMedium
                    color: Colors.textOnPrimary
                }
            }
        }

        // Map placeholder
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 12
            color: Colors.surface
            border.color: Colors.border

            // Map would be integrated here
            ColumnLayout {
                anchors.centerIn: parent
                spacing: Spacing.medium

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "ODD Map View"
                    font: Typography.headingSmall
                    color: Colors.textSecondary
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "(Integrate with Navigation/Maps)"
                    font: Typography.bodySmall
                    color: Colors.textTertiary
                }

                // Legend
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Spacing.large

                    LegendItem { color: Colors.success; text: "Full ODD" }
                    LegendItem { color: Colors.warning; text: "Limited ODD" }
                    LegendItem { color: Colors.error; text: "No ODD" }
                }
            }
        }

        // ODD Constraints
        Rectangle {
            Layout.fillWidth: true
            height: constraintsLayout.height + Spacing.medium * 2
            radius: 12
            color: Colors.surface

            RowLayout {
                id: constraintsLayout
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: Spacing.medium
                spacing: Spacing.large

                // Road Type
                ConstraintItem {
                    title: "Road Type"
                    value: root.roadType
                    icon: "🛣️"
                    supported: true
                }

                // Speed Range
                ConstraintItem {
                    title: "Speed Range"
                    value: root.minSpeedKph + " - " + root.maxSpeedKph + " km/h"
                    icon: "🚗"
                    supported: true
                }

                // Weather Limit
                ConstraintItem {
                    title: "Weather Limit"
                    value: root.weatherLimit
                    icon: "🌤️"
                    supported: true
                }

                // Geofence
                ConstraintItem {
                    title: "Region"
                    value: "Enabled"
                    icon: "📍"
                    supported: true
                }
            }
        }

        // ODD Reasons (if outside or approaching)
        Rectangle {
            Layout.fillWidth: true
            visible: root.oddReasons.length > 0 || root.oddStatus >= 2
            height: reasonsColumn.height + Spacing.medium * 2
            radius: 12
            color: root.oddStatus === 3 ? Colors.errorBackground : Colors.warningBackground
            border.color: root.oddStatus === 3 ? Colors.error : Colors.warning
            border.width: 1

            ColumnLayout {
                id: reasonsColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Spacing.medium
                spacing: Spacing.small

                Text {
                    text: root.oddStatus === 3 ? "Outside ODD" : "Approaching ODD Boundary"
                    font: Typography.labelMedium
                    color: root.oddStatus === 3 ? Colors.error : Colors.warning
                }

                Repeater {
                    model: root.oddReasons

                    Text {
                        text: "• " + modelData
                        font: Typography.bodySmall
                        color: Colors.textSecondary
                    }
                }
            }
        }
    }

    // Helper functions
    function getStatusColor() {
        switch (root.oddStatus) {
            case 1: return Colors.success
            case 2: return Colors.warning
            case 3: return Colors.error
            default: return Colors.textTertiary
        }
    }

    function getStatusText() {
        switch (root.oddStatus) {
            case 1: return "Inside ODD"
            case 2: return "Approaching Boundary"
            case 3: return "Outside ODD"
            case 4: return "ODD Uncertain"
            default: return "Unknown"
        }
    }

    // Legend item component
    component LegendItem: Row {
        property color color: Colors.success
        property string text: ""
        spacing: Spacing.xsmall

        Rectangle {
            width: 16
            height: 16
            radius: 4
            color: parent.color
        }

        Text {
            text: parent.text
            font: Typography.caption
            color: Colors.textSecondary
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // Constraint item component
    component ConstraintItem: ColumnLayout {
        property string title: ""
        property string value: ""
        property string icon: ""
        property bool supported: true

        spacing: Spacing.xsmall

        RowLayout {
            spacing: Spacing.xsmall

            Text {
                text: icon
                font.pixelSize: 16
            }

            Text {
                text: title
                font: Typography.caption
                color: Colors.textSecondary
            }
        }

        Text {
            text: value
            font: Typography.bodyMedium
            color: supported ? Colors.textPrimary : Colors.error
        }
    }
}
