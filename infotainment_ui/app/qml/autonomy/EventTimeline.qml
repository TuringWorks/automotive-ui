// EventTimeline.qml
// ADAS Event Timeline Display
// Part of: Infotainment Autonomy Center
// Spec: FR-INF-ADAS-003

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import DesignSystem 1.0

/**
 * Event Timeline - Shows ADAS engagement history and disengagement causes.
 *
 * Displays:
 * - Engagement/disengagement events
 * - Takeover requests and responses
 * - System limitations and faults
 * - Driver monitoring events
 */
Rectangle {
    id: root

    property var events: []

    // Filters
    property bool showEngagements: true
    property bool showDisengagements: true
    property bool showTakeovers: true
    property bool showWarnings: true

    radius: 12
    color: Colors.surfaceSecondary

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.large
        spacing: Spacing.medium

        // Header with filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.medium

            Text {
                text: "Event Timeline"
                font: Typography.headingMedium
                color: Colors.textPrimary
            }

            Item { Layout.fillWidth: true }

            // Filter buttons
            Row {
                spacing: Spacing.small

                FilterChip {
                    text: "Engagements"
                    active: root.showEngagements
                    onClicked: root.showEngagements = !root.showEngagements
                }

                FilterChip {
                    text: "Disengagements"
                    active: root.showDisengagements
                    onClicked: root.showDisengagements = !root.showDisengagements
                }

                FilterChip {
                    text: "Takeovers"
                    active: root.showTakeovers
                    onClicked: root.showTakeovers = !root.showTakeovers
                }

                FilterChip {
                    text: "Warnings"
                    active: root.showWarnings
                    onClicked: root.showWarnings = !root.showWarnings
                }
            }
        }

        // Event list
        ListView {
            id: eventList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Spacing.small

            model: filteredEvents()

            delegate: EventItem {
                width: eventList.width
                eventData: modelData
            }

            // Empty state
            Text {
                anchors.centerIn: parent
                visible: eventList.count === 0
                text: "No events to display"
                font: Typography.bodyMedium
                color: Colors.textTertiary
            }
        }

        // Summary stats
        RowLayout {
            Layout.fillWidth: true
            spacing: Spacing.large

            Text {
                text: "Total: " + root.events.length + " events"
                font: Typography.caption
                color: Colors.textSecondary
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Last 24 hours"
                font: Typography.caption
                color: Colors.textTertiary
            }
        }
    }

    // Filter function
    function filteredEvents() {
        if (!root.events) return []

        return root.events.filter(function(event) {
            const cat = event.category || 0
            switch (cat) {
                case 1: // Engagement
                    return root.showEngagements || root.showDisengagements
                case 2: // Takeover
                    return root.showTakeovers
                case 6: // Fault
                case 3: // Perception
                    return root.showWarnings
                default:
                    return true
            }
        })
    }

    // Filter chip component
    component FilterChip: Rectangle {
        property string text: ""
        property bool active: true
        signal clicked()

        width: chipText.width + Spacing.medium * 2
        height: 28
        radius: 14
        color: active ? Colors.primaryLight : Colors.surfaceTertiary
        border.color: active ? Colors.primary : Colors.border
        border.width: 1

        Text {
            id: chipText
            anchors.centerIn: parent
            text: parent.text
            font: Typography.labelSmall
            color: active ? Colors.primary : Colors.textSecondary
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    // Event item component
    component EventItem: Rectangle {
        property var eventData: ({})

        height: eventContent.height + Spacing.medium * 2
        radius: 8
        color: Colors.surface
        border.color: getEventBorderColor()
        border.width: 1

        RowLayout {
            id: eventContent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: Spacing.medium
            spacing: Spacing.medium

            // Timeline dot
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: getEventColor()
            }

            // Event content
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: eventData.summary || "Event"
                        font: Typography.bodySmall
                        color: Colors.textPrimary
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: formatTime(eventData.timestampMs || 0)
                        font: Typography.caption
                        color: Colors.textTertiary
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: eventData.details || ""
                    font: Typography.caption
                    color: Colors.textSecondary
                    wrapMode: Text.WordWrap
                    visible: text.length > 0
                }
            }

            // Severity indicator
            Rectangle {
                visible: (eventData.severity || 0) >= 3
                width: 24
                height: 24
                radius: 4
                color: Colors.errorBackground

                Text {
                    anchors.centerIn: parent
                    text: "!"
                    font: Typography.labelSmall
                    color: Colors.error
                }
            }
        }

        function getEventColor() {
            const cat = eventData.category || 0
            switch (cat) {
                case 0: return Colors.info      // StateChange
                case 1: return Colors.success   // Engagement
                case 2: return Colors.warning   // Takeover
                case 3: return Colors.info      // Perception
                case 4: return Colors.info      // ODD
                case 5: return Colors.warning   // DMS
                case 6: return Colors.error     // Fault
                case 7: return Colors.error     // Security
                default: return Colors.textTertiary
            }
        }

        function getEventBorderColor() {
            const severity = eventData.severity || 0
            if (severity >= 4) return Colors.error
            if (severity >= 3) return Colors.warning
            return Colors.border
        }

        function formatTime(timestampMs) {
            if (!timestampMs) return ""
            const date = new Date(timestampMs)
            return date.toLocaleTimeString(Qt.locale(), "HH:mm:ss")
        }
    }
}
