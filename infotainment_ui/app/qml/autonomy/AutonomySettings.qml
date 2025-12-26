// AutonomySettings.qml
// ADAS/Autonomy Settings Page
// Part of: Infotainment Autonomy Center

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import DesignSystem 1.0

/**
 * Autonomy Settings - Configure ADAS features and preferences.
 *
 * Settings:
 * - Feature toggles (requires VehicleSettingsWrite permission)
 * - Alert preferences
 * - DMS sensitivity
 * - Calibration status
 */
Rectangle {
    id: root

    property bool hasWritePermission: true

    color: Colors.surfaceSecondary

    ScrollView {
        anchors.fill: parent
        anchors.margins: Spacing.large
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: Spacing.large

            // Feature Toggles
            SettingsSection {
                Layout.fillWidth: true
                title: "Feature Settings"
                description: "Enable or disable ADAS features"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Spacing.medium

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Adaptive Cruise Control"
                        description: "Automatically maintain speed and distance"
                        checked: true
                        enabled: root.hasWritePermission
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Lane Keeping Assist"
                        description: "Provide steering input to stay in lane"
                        checked: true
                        enabled: root.hasWritePermission
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Lane Centering"
                        description: "Keep vehicle centered in lane"
                        checked: true
                        enabled: root.hasWritePermission
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Blind Spot Monitoring"
                        description: "Alert when vehicles are in blind spots"
                        checked: true
                        enabled: root.hasWritePermission
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Auto Emergency Braking"
                        description: "Automatically brake to avoid collisions"
                        checked: true
                        enabled: root.hasWritePermission
                    }
                }
            }

            // Alert Preferences
            SettingsSection {
                Layout.fillWidth: true
                title: "Alert Preferences"
                description: "Configure alert sounds and sensitivity"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Spacing.medium

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Lane Departure Warning Volume"
                            font: Typography.bodyMedium
                            color: Colors.textPrimary
                            Layout.fillWidth: true
                        }

                        Slider {
                            from: 0
                            to: 100
                            value: 70
                            Layout.preferredWidth: 150
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Forward Collision Warning Sensitivity"
                            font: Typography.bodyMedium
                            color: Colors.textPrimary
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            model: ["Early", "Normal", "Late"]
                            currentIndex: 1
                            Layout.preferredWidth: 120
                        }
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Haptic Alerts"
                        description: "Use seat vibration for alerts"
                        checked: true
                    }
                }
            }

            // Driver Monitoring
            SettingsSection {
                Layout.fillWidth: true
                title: "Driver Monitoring"
                description: "Settings for attention monitoring"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Spacing.medium

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Hands-On Reminder Timing"
                            font: Typography.bodyMedium
                            color: Colors.textPrimary
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            model: ["5 seconds", "10 seconds", "15 seconds"]
                            currentIndex: 1
                            Layout.preferredWidth: 120
                        }
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Distraction Warnings"
                        description: "Alert when looking away from road"
                        checked: true
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Drowsiness Detection"
                        description: "Detect signs of fatigue"
                        checked: true
                    }
                }
            }

            // Calibration Status
            SettingsSection {
                Layout.fillWidth: true
                title: "System Status"
                description: "Sensor calibration and system health"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Spacing.medium

                    StatusRow {
                        Layout.fillWidth: true
                        title: "Camera Calibration"
                        status: "Complete"
                        statusColor: Colors.success
                    }

                    StatusRow {
                        Layout.fillWidth: true
                        title: "Radar Calibration"
                        status: "Complete"
                        statusColor: Colors.success
                    }

                    StatusRow {
                        Layout.fillWidth: true
                        title: "Map Data"
                        status: "Updated"
                        statusColor: Colors.success
                    }

                    StatusRow {
                        Layout.fillWidth: true
                        title: "Software Version"
                        status: "2.1.4"
                        statusColor: Colors.textSecondary
                    }

                    Button {
                        Layout.alignment: Qt.AlignRight
                        text: "Check for Updates"
                        onClicked: console.log("Check updates")
                    }
                }
            }

            // Privacy
            SettingsSection {
                Layout.fillWidth: true
                title: "Privacy"
                description: "Data collection and sharing"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Spacing.medium

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Share Driving Data"
                        description: "Help improve the system (anonymized)"
                        checked: false
                    }

                    SettingToggle {
                        Layout.fillWidth: true
                        title: "Store Event Logs"
                        description: "Keep detailed logs for troubleshooting"
                        checked: true
                    }

                    Button {
                        Layout.alignment: Qt.AlignRight
                        text: "View Privacy Policy"
                        flat: true
                    }
                }
            }
        }
    }

    // Settings section component
    component SettingsSection: Rectangle {
        property string title: ""
        property string description: ""
        default property alias content: contentLoader.sourceComponent

        implicitHeight: sectionColumn.height + Spacing.large * 2
        radius: 12
        color: Colors.surface

        ColumnLayout {
            id: sectionColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: Spacing.large
            spacing: Spacing.medium

            Text {
                text: title
                font: Typography.headingSmall
                color: Colors.textPrimary
            }

            Text {
                text: description
                font: Typography.bodySmall
                color: Colors.textSecondary
                Layout.fillWidth: true
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Colors.border
            }

            Loader {
                id: contentLoader
                Layout.fillWidth: true
            }
        }
    }

    // Setting toggle component
    component SettingToggle: RowLayout {
        property string title: ""
        property string description: ""
        property bool checked: false

        spacing: Spacing.medium

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: title
                font: Typography.bodyMedium
                color: Colors.textPrimary
            }

            Text {
                text: description
                font: Typography.caption
                color: Colors.textSecondary
            }
        }

        Switch {
            checked: parent.checked
            onCheckedChanged: parent.checked = checked
        }
    }

    // Status row component
    component StatusRow: RowLayout {
        property string title: ""
        property string status: ""
        property color statusColor: Colors.success

        spacing: Spacing.medium

        Text {
            text: title
            font: Typography.bodyMedium
            color: Colors.textPrimary
            Layout.fillWidth: true
        }

        Text {
            text: status
            font: Typography.labelMedium
            color: statusColor
        }
    }
}
