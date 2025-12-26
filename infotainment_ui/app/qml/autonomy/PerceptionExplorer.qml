// PerceptionExplorer.qml
// Engineering-Only Perception Explorer
// Part of: Infotainment Autonomy Center
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 5.1, 10.6

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import DesignSystem 1.0

/**
 * Perception Explorer - Engineering/diagnostic view.
 *
 * Features:
 * - Event List Simulator controls (play/pause/seek)
 * - Environment preset toggles
 * - Weather particle toggles
 * - Quality manager debug overlay
 *
 * Security: CR-INF-ADAS-120
 * - Must be authorization-gated
 * - Must be motion-locked (disabled when driving)
 */
Rectangle {
    id: root

    property bool vehicleInMotion: false
    property bool simulationActive: false

    // Quality settings
    property int qualityLevel: 1
    property bool reflectionsEnabled: true
    property bool particlesEnabled: true

    // Environment
    property int environmentPreset: 1  // 0=Dawn, 1=Day, 2=Dusk, 3=Night
    property int weatherCondition: 0   // 0=Clear, 1=Rain, 2=Snow, 3=Fog

    // Simulation
    property double simulationTime: 0
    property double simulationDuration: 60

    signal playSimulation()
    signal pauseSimulation()
    signal seekSimulation(real time)
    signal loadScenario(string path)

    color: Colors.surface
    opacity: root.vehicleInMotion ? 0.5 : 1.0

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Spacing.large
        spacing: Spacing.large
        enabled: !root.vehicleInMotion

        // Warning banner
        Rectangle {
            Layout.fillWidth: true
            height: 48
            radius: 8
            color: Colors.warningBackground
            border.color: Colors.warning
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: Spacing.medium
                spacing: Spacing.small

                Text {
                    text: "!"
                    font: Typography.headingSmall
                    color: Colors.warning
                }

                Text {
                    text: "Engineering Mode - For Development Use Only"
                    font: Typography.bodyMedium
                    color: Colors.warning
                }

                Item { Layout.fillWidth: true }

                Text {
                    visible: root.vehicleInMotion
                    text: "MOTION LOCKED"
                    font: Typography.labelMedium
                    color: Colors.error
                }
            }
        }

        // Main content
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Spacing.large

            // Left panel - Controls
            ColumnLayout {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                spacing: Spacing.medium

                // Environment Presets (Eos-aligned)
                GroupBox {
                    Layout.fillWidth: true
                    title: "Environment Preset"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Spacing.small

                        Repeater {
                            model: ["Dawn", "Day", "Dusk", "Night"]

                            RadioButton {
                                text: modelData
                                checked: root.environmentPreset === index
                                onClicked: root.environmentPreset = index
                            }
                        }
                    }
                }

                // Weather Particles
                GroupBox {
                    Layout.fillWidth: true
                    title: "Weather Effects"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Spacing.small

                        Repeater {
                            model: ["Clear", "Rain", "Snow", "Fog"]

                            RadioButton {
                                text: modelData
                                checked: root.weatherCondition === index
                                onClicked: root.weatherCondition = index
                            }
                        }

                        CheckBox {
                            text: "Particles Enabled"
                            checked: root.particlesEnabled
                            onCheckedChanged: root.particlesEnabled = checked
                        }
                    }
                }

                // Quality Settings
                GroupBox {
                    Layout.fillWidth: true
                    title: "Quality Settings"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Spacing.small

                        Repeater {
                            model: ["Ultra", "High", "Medium", "Low", "Minimal"]

                            RadioButton {
                                text: modelData
                                checked: root.qualityLevel === index
                                onClicked: root.qualityLevel = index
                            }
                        }

                        CheckBox {
                            text: "Reflections"
                            checked: root.reflectionsEnabled
                            onCheckedChanged: root.reflectionsEnabled = checked
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }

            // Center - 3D View placeholder
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 12
                color: Colors.surfaceSecondary
                border.color: Colors.border

                Text {
                    anchors.centerIn: parent
                    text: "3D Perception View\n(Connect to AdasScene3D)"
                    font: Typography.bodyMedium
                    color: Colors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Right panel - Scenario Replay
            ColumnLayout {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                spacing: Spacing.medium

                // Scenario Replay Controls
                GroupBox {
                    Layout.fillWidth: true
                    title: "Event List Simulator"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Spacing.medium

                        // Scenario selection
                        ComboBox {
                            Layout.fillWidth: true
                            model: [
                                "Lane Dropout",
                                "Lead Vehicle Cut-in",
                                "Heavy Rain",
                                "Night Reflections",
                                "ODD Boundary Crossing",
                                "Takeover No Response"
                            ]
                        }

                        Button {
                            Layout.fillWidth: true
                            text: "Load Scenario"
                            onClicked: root.loadScenario("selected")
                        }

                        // Timeline
                        Slider {
                            Layout.fillWidth: true
                            from: 0
                            to: root.simulationDuration
                            value: root.simulationTime
                            onMoved: root.seekSimulation(value)
                        }

                        // Time display
                        RowLayout {
                            Layout.fillWidth: true

                            Text {
                                text: formatTime(root.simulationTime)
                                font: Typography.caption
                                color: Colors.textSecondary
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: formatTime(root.simulationDuration)
                                font: Typography.caption
                                color: Colors.textSecondary
                            }
                        }

                        // Playback controls
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Spacing.small

                            Button {
                                Layout.fillWidth: true
                                text: root.simulationActive ? "Pause" : "Play"
                                onClicked: {
                                    if (root.simulationActive) {
                                        root.pauseSimulation()
                                    } else {
                                        root.playSimulation()
                                    }
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                text: "Reset"
                                onClicked: root.seekSimulation(0)
                            }
                        }
                    }
                }

                // Debug Info
                GroupBox {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: "Debug Info"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Spacing.xsmall

                        Text {
                            text: "FPS: 60"
                            font: Typography.caption
                            color: Colors.textSecondary
                        }

                        Text {
                            text: "Objects: 12"
                            font: Typography.caption
                            color: Colors.textSecondary
                        }

                        Text {
                            text: "Particles: 5000"
                            font: Typography.caption
                            color: Colors.textSecondary
                        }

                        Text {
                            text: "Quality: " + ["Ultra", "High", "Medium", "Low", "Minimal"][root.qualityLevel]
                            font: Typography.caption
                            color: Colors.textSecondary
                        }

                        Text {
                            text: "Reflections: " + (root.reflectionsEnabled ? "On" : "Off")
                            font: Typography.caption
                            color: Colors.textSecondary
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }
    }

    function formatTime(seconds) {
        const mins = Math.floor(seconds / 60)
        const secs = Math.floor(seconds % 60)
        return mins.toString().padStart(2, '0') + ":" + secs.toString().padStart(2, '0')
    }
}
