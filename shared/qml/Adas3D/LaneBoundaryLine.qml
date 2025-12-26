// LaneBoundaryLine.qml
// Lane Boundary Visualization with Confidence-Based Styling
// Part of: Shared ADAS 3D Components
// Spec: SR-CL-ADAS-141 - Lane visualization reflects quality

import QtQuick
import QtQuick3D

/**
 * Renders a single lane boundary line with confidence indication.
 *
 * Confidence levels map to visual styles:
 * - High (3): Solid green line
 * - Medium (2): Dashed yellow line
 * - Low (1): Dashed orange line
 * - NotAvailable (0): Grey dashed line
 *
 * Line types (from LaneType enum):
 * - 0: Unknown
 * - 1: Solid
 * - 2: Dashed
 * - 3: DoubleSolid
 * - 4: DashedSolid
 * - 5: SolidDashed
 * - 6: RoadEdge
 * - 7: Virtual
 */
Node {
    id: root

    property real lineLength: 100.0
    property int confidence: 0       // ConfidenceLevel enum
    property int lineType: 0         // LaneType enum
    property int qualityLevel: 1
    property int environmentPreset: 1

    // Line styling
    property real lineWidth: 0.12    // 12cm standard lane marking

    // Confidence colors
    readonly property var confidenceColors: [
        "#666666",   // NotAvailable - grey
        "#ff6600",   // Low - orange
        "#ffff00",   // Medium - yellow
        "#00ff00"    // High - green
    ]

    // Determine if line should be dashed
    property bool isDashed: {
        if (root.confidence < 2) return true  // Low confidence always dashed
        switch (root.lineType) {
            case 2: return true   // Dashed
            case 4: return true   // DashedSolid (show dashed side)
            case 7: return true   // Virtual
            default: return false
        }
    }

    // Get line color based on confidence
    function getLineColor() {
        const baseColor = root.confidenceColors[Math.min(root.confidence, 3)]
        return baseColor
    }

    // Get emissive factor for night visibility
    function getEmissiveFactor() {
        if (root.environmentPreset !== 3) return Qt.vector3d(0, 0, 0)

        switch (root.confidence) {
            case 3: return Qt.vector3d(0, 0.3, 0)      // Green glow
            case 2: return Qt.vector3d(0.3, 0.3, 0)    // Yellow glow
            case 1: return Qt.vector3d(0.3, 0.15, 0)   // Orange glow
            default: return Qt.vector3d(0.1, 0.1, 0.1) // Dim grey
        }
    }

    // Solid line rendering
    Model {
        id: solidLine
        visible: !root.isDashed
        source: "#Rectangle"
        scale: Qt.vector3d(root.lineWidth, root.lineLength, 1)
        eulerRotation.x: -90
        position: Qt.vector3d(0, 0, root.lineLength / 2)

        materials: [
            PrincipledMaterial {
                baseColor: root.getLineColor()
                roughness: 0.5
                emissiveFactor: root.getEmissiveFactor()
            }
        ]
    }

    // Dashed line rendering
    Node {
        id: dashedLine
        visible: root.isDashed

        Repeater3D {
            id: dashes
            model: Math.floor(root.lineLength / 3)  // Dash every 3 meters

            Model {
                required property int index
                source: "#Rectangle"
                scale: Qt.vector3d(root.lineWidth, 1.5, 1)  // 1.5m dash length
                eulerRotation.x: -90
                position: Qt.vector3d(0, 0, index * 3 + 1.5)

                materials: [
                    PrincipledMaterial {
                        baseColor: root.getLineColor()
                        roughness: 0.5
                        emissiveFactor: root.getEmissiveFactor()
                    }
                ]
            }
        }
    }

    // Double line (if applicable)
    Node {
        id: doubleLine
        visible: root.lineType === 3  // DoubleSolid

        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(root.lineWidth, root.lineLength, 1)
            eulerRotation.x: -90
            position: Qt.vector3d(-0.2, 0, root.lineLength / 2)

            materials: [
                PrincipledMaterial {
                    baseColor: root.getLineColor()
                    roughness: 0.5
                    emissiveFactor: root.getEmissiveFactor()
                }
            ]
        }

        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(root.lineWidth, root.lineLength, 1)
            eulerRotation.x: -90
            position: Qt.vector3d(0.2, 0, root.lineLength / 2)

            materials: [
                PrincipledMaterial {
                    baseColor: root.getLineColor()
                    roughness: 0.5
                    emissiveFactor: root.getEmissiveFactor()
                }
            ]
        }
    }

    // Confidence indicator dots (for low confidence)
    Node {
        id: uncertaintyIndicator
        visible: root.confidence <= 1 && root.qualityLevel <= 2

        Repeater3D {
            model: 5

            Model {
                required property int index
                source: "#Sphere"
                scale: Qt.vector3d(0.1, 0.1, 0.1)
                position: Qt.vector3d(
                    0,
                    0.1,
                    index * 20 + 10
                )

                materials: [
                    PrincipledMaterial {
                        baseColor: "#ff0000"
                        roughness: 0.5
                        emissiveFactor: Qt.vector3d(0.5, 0, 0)
                    }
                ]
            }
        }
    }
}
