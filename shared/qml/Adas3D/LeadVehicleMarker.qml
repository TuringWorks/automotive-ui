// LeadVehicleMarker.qml
// Lead Vehicle Visualization for ADAS 3D Scene
// Part of: Shared ADAS 3D Components
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.5

import QtQuick
import QtQuick3D

/**
 * Visualizes the detected lead vehicle.
 *
 * Shows:
 * - Vehicle bounding box representation
 * - Distance indicator
 * - Relative speed indication (color)
 * - Time gap visualization
 */
Node {
    id: root

    // Lead vehicle data
    property real distance: 0           // meters
    property real lateralOffset: 0      // meters
    property real relativeSpeed: 0      // m/s (negative = closing)
    property bool isClosing: false
    property real timeGap: 0            // seconds

    // Styling
    property color closingColor: "#ff4444"
    property color matchingColor: "#44ff44"
    property color divergingColor: "#4444ff"

    // Calculate position
    property real zPosition: Math.max(5, distance)

    // Get color based on relative speed
    function getStateColor() {
        if (root.relativeSpeed < -1.0) return root.closingColor
        if (root.relativeSpeed > 1.0) return root.divergingColor
        return root.matchingColor
    }

    // Main vehicle representation
    Node {
        id: vehicleNode
        position: Qt.vector3d(root.lateralOffset, 0, root.zPosition)

        // Vehicle box (simplified car shape)
        Model {
            id: vehicleBody
            source: "#Cube"
            scale: Qt.vector3d(1.8, 1.2, 4.5)  // Car-like proportions
            position: Qt.vector3d(0, 0.7, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: "#303040"
                    roughness: 0.7
                    metalness: 0.3
                }
            ]
        }

        // Vehicle roof
        Model {
            source: "#Cube"
            scale: Qt.vector3d(1.6, 0.6, 2.5)
            position: Qt.vector3d(0, 1.5, 0.3)

            materials: [
                PrincipledMaterial {
                    baseColor: "#252535"
                    roughness: 0.6
                    metalness: 0.2
                }
            ]
        }

        // Tail lights (show state)
        Model {
            id: leftTaillight
            source: "#Cube"
            scale: Qt.vector3d(0.3, 0.2, 0.1)
            position: Qt.vector3d(-0.7, 0.6, -2.3)

            materials: [
                PrincipledMaterial {
                    baseColor: root.getStateColor()
                    roughness: 0.3
                    emissiveFactor: Qt.vector3d(0.8, 0.2, 0.2)
                }
            ]
        }

        Model {
            id: rightTaillight
            source: "#Cube"
            scale: Qt.vector3d(0.3, 0.2, 0.1)
            position: Qt.vector3d(0.7, 0.6, -2.3)

            materials: [
                PrincipledMaterial {
                    baseColor: root.getStateColor()
                    roughness: 0.3
                    emissiveFactor: Qt.vector3d(0.8, 0.2, 0.2)
                }
            ]
        }

        // Wheels
        Repeater3D {
            model: [
                Qt.vector3d(-0.85, 0.35, 1.3),   // Front left
                Qt.vector3d(0.85, 0.35, 1.3),    // Front right
                Qt.vector3d(-0.85, 0.35, -1.3),  // Rear left
                Qt.vector3d(0.85, 0.35, -1.3)    // Rear right
            ]

            Model {
                required property vector3d modelData
                source: "#Cylinder"
                scale: Qt.vector3d(0.35, 0.2, 0.35)
                eulerRotation.z: 90
                position: modelData

                materials: [
                    PrincipledMaterial {
                        baseColor: "#1a1a1a"
                        roughness: 0.9
                    }
                ]
            }
        }
    }

    // Distance indicator line (on ground)
    Node {
        id: distanceIndicator

        // Line from ego to lead vehicle
        Repeater3D {
            model: Math.max(1, Math.floor(root.distance / 5))  // Marker every 5m

            Model {
                required property int index
                source: "#Rectangle"
                scale: Qt.vector3d(0.5, 0.5, 1)
                eulerRotation.x: -90
                position: Qt.vector3d(0, 0.02, index * 5 + 2.5)
                opacity: 0.6

                materials: [
                    PrincipledMaterial {
                        baseColor: root.getStateColor()
                        roughness: 0.8
                    }
                ]
            }
        }
    }

    // Bounding box outline (highlight when closing)
    Node {
        id: boundingBox
        visible: root.isClosing
        position: Qt.vector3d(root.lateralOffset, 0.7, root.zPosition)

        // Corner markers
        Repeater3D {
            model: [
                Qt.vector3d(-1.0, -0.7, -2.4),
                Qt.vector3d(1.0, -0.7, -2.4),
                Qt.vector3d(-1.0, 0.7, -2.4),
                Qt.vector3d(1.0, 0.7, -2.4)
            ]

            Model {
                required property vector3d modelData
                source: "#Sphere"
                scale: Qt.vector3d(0.08, 0.08, 0.08)
                position: modelData

                materials: [
                    PrincipledMaterial {
                        baseColor: root.closingColor
                        roughness: 0.3
                        emissiveFactor: Qt.vector3d(1, 0.3, 0.3)
                    }
                ]
            }
        }
    }

    // Distance text plane (billboard)
    Node {
        id: distanceLabel
        position: Qt.vector3d(root.lateralOffset, 3.5, root.zPosition)

        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(2.5, 0.8, 1)
            opacity: 0.85

            materials: [
                PrincipledMaterial {
                    baseColor: "#202020"
                    roughness: 1.0
                }
            ]
        }

        // Note: Actual text would require a Texture with rendered text
        // or a 3D text component. This is a placeholder.
    }

    // Animation for closing warning
    SequentialAnimation {
        running: root.isClosing
        loops: Animation.Infinite

        NumberAnimation {
            target: boundingBox
            property: "opacity"
            from: 1.0
            to: 0.3
            duration: 500
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: boundingBox
            property: "opacity"
            from: 0.3
            to: 1.0
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }
}
