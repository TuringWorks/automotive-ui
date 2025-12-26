// RoadVisualization.qml
// Road and Lane Visualization for ADAS 3D Scene
// Part of: Shared ADAS 3D Components
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.3

import QtQuick
import QtQuick3D

/**
 * Renders road surface and lane markings.
 *
 * Features:
 * - Road ribbon with asphalt texture
 * - Lane lines with confidence-based styling
 * - Drivable corridor overlay
 *
 * Safety Requirements:
 * - SR-CL-ADAS-141: Lane visualization reflects quality
 * - Lane confidence shown via line style (solid/dashed/greyed)
 */
Node {
    id: root

    property var laneModel: null
    property var corridorData: null
    property int qualityLevel: 1
    property int environmentPreset: 1

    // Road dimensions
    property real roadWidth: 7.0        // meters (2 lanes)
    property real roadLength: 150.0     // meters ahead
    property real laneWidth: 3.5        // meters per lane

    // Lane quality colors
    readonly property color laneColorHigh: "#00ff00"      // Green - high confidence
    readonly property color laneColorMedium: "#ffff00"    // Yellow - medium confidence
    readonly property color laneColorLow: "#ff6600"       // Orange - low confidence
    readonly property color laneColorInvalid: "#666666"   // Grey - no data

    // Road surface
    Model {
        id: roadSurface
        source: "#Rectangle"
        scale: Qt.vector3d(root.roadWidth, root.roadLength, 1)
        eulerRotation.x: -90
        position: Qt.vector3d(0, 0.01, root.roadLength / 2)

        materials: [
            PrincipledMaterial {
                id: roadMaterial
                baseColor: getRoadColor()
                roughness: 0.9
                metalness: 0.0
            }
        ]

        function getRoadColor() {
            switch (root.environmentPreset) {
                case 0: return "#2a2a2a"  // Dawn - dark grey
                case 1: return "#333333"  // Day - asphalt grey
                case 2: return "#282828"  // Dusk - darker
                case 3: return "#1a1a1a"  // Night - very dark
                default: return "#333333"
            }
        }
    }

    // Center line (dashed)
    Node {
        id: centerLine
        visible: true

        Repeater3D {
            id: centerLineDashes
            model: Math.floor(root.roadLength / 4)  // Dash every 4 meters

            Model {
                required property int index
                source: "#Rectangle"
                scale: Qt.vector3d(0.15, 2.0, 1)  // 15cm wide, 2m long
                eulerRotation.x: -90
                position: Qt.vector3d(0, 0.02, index * 4 + 2)

                materials: [
                    PrincipledMaterial {
                        baseColor: "#ffffff"
                        roughness: 0.5
                        emissiveFactor: root.environmentPreset === 3 ?
                            Qt.vector3d(0.3, 0.3, 0.3) : Qt.vector3d(0, 0, 0)
                    }
                ]
            }
        }
    }

    // Left lane boundary
    LaneBoundaryLine {
        id: leftLaneLine
        visible: root.laneModel !== null
        position: Qt.vector3d(-root.laneWidth, 0.02, 0)
        lineLength: root.roadLength
        confidence: root.laneModel ? root.laneModel.left.confidence : 0
        lineType: root.laneModel ? root.laneModel.left.type : 0
        qualityLevel: root.qualityLevel
        environmentPreset: root.environmentPreset
    }

    // Right lane boundary
    LaneBoundaryLine {
        id: rightLaneLine
        visible: root.laneModel !== null
        position: Qt.vector3d(root.laneWidth, 0.02, 0)
        lineLength: root.roadLength
        confidence: root.laneModel ? root.laneModel.right.confidence : 0
        lineType: root.laneModel ? root.laneModel.right.type : 0
        qualityLevel: root.qualityLevel
        environmentPreset: root.environmentPreset
    }

    // Road edge lines (solid)
    Model {
        id: leftEdge
        source: "#Rectangle"
        scale: Qt.vector3d(0.1, root.roadLength, 1)
        eulerRotation.x: -90
        position: Qt.vector3d(-root.roadWidth / 2, 0.02, root.roadLength / 2)

        materials: [
            PrincipledMaterial {
                baseColor: "#ffffff"
                roughness: 0.5
            }
        ]
    }

    Model {
        id: rightEdge
        source: "#Rectangle"
        scale: Qt.vector3d(0.1, root.roadLength, 1)
        eulerRotation.x: -90
        position: Qt.vector3d(root.roadWidth / 2, 0.02, root.roadLength / 2)

        materials: [
            PrincipledMaterial {
                baseColor: "#ffffff"
                roughness: 0.5
            }
        ]
    }

    // Drivable corridor overlay (when available)
    Node {
        id: corridorOverlay
        visible: root.corridorData !== null && root.corridorData.isValid

        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(
                root.corridorData ? root.corridorData.corridorWidthM : 2.0,
                root.corridorData ? Math.min(root.corridorData.pathLengthM, 100) : 50,
                1
            )
            eulerRotation.x: -90
            position: Qt.vector3d(0, 0.015, 25)
            opacity: 0.3

            materials: [
                PrincipledMaterial {
                    baseColor: "#00aaff"
                    roughness: 1.0
                    opacity: 0.3
                }
            ]
        }
    }

    // Lateral offset indicator
    Model {
        id: egoPosition
        source: "#Rectangle"
        scale: Qt.vector3d(0.3, 0.3, 1)
        eulerRotation.x: -90
        position: Qt.vector3d(
            root.laneModel ? root.laneModel.lateralOffsetM : 0,
            0.03,
            3
        )

        materials: [
            PrincipledMaterial {
                baseColor: "#00ff00"
                roughness: 0.5
                emissiveFactor: Qt.vector3d(0.5, 1.0, 0.5)
            }
        ]
    }
}
