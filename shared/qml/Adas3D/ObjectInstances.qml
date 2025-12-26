// ObjectInstances.qml
// Instanced Object Rendering for ADAS Visualization
// Part of: Shared ADAS 3D Components
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.3

import QtQuick
import QtQuick3D

/**
 * Renders detected objects using instancing for performance.
 *
 * Object types:
 * - Vehicles (cars, trucks, motorcycles)
 * - Pedestrians
 * - Cyclists
 * - Static objects (cones, barriers)
 *
 * Uses Qt Quick 3D instancing to minimize draw calls.
 */
Node {
    id: root

    property var objectList: null
    property int qualityLevel: 1

    signal objectClicked(int objectId)

    // Maximum instances by quality level
    property int maxInstances: {
        switch (root.qualityLevel) {
            case 0: return 100    // Ultra
            case 1: return 50     // High
            case 2: return 30     // Medium
            case 3: return 15     // Low
            default: return 10    // Minimal
        }
    }

    // Inline component definitions (must be inside root Node)

    component VehicleModel: Node {
        id: vehicleNode

        property int objectId: 0
        property int objectType: 1  // 1=Car, 2=Truck, 3=Motorcycle
        property bool isRelevant: false
        property bool isThreat: false
        property int qualityLevel: 1

        // Scale based on vehicle type
        property vector3d vehicleScale: {
            switch (objectType) {
                case 2: return Qt.vector3d(2.5, 2.0, 6.0)   // Truck
                case 3: return Qt.vector3d(0.8, 1.0, 2.0)   // Motorcycle
                default: return Qt.vector3d(1.8, 1.2, 4.5)  // Car
            }
        }

        Model {
            source: "#Cube"
            scale: vehicleNode.vehicleScale
            position: Qt.vector3d(0, vehicleNode.vehicleScale.y / 2, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: vehicleNode.isThreat ? "#ff4444" :
                               (vehicleNode.isRelevant ? "#4488ff" : "#606060")
                    roughness: 0.6
                    metalness: 0.3
                }
            ]
        }

        // Threat indicator glow
        Model {
            visible: vehicleNode.isThreat
            source: "#Cube"
            scale: Qt.vector3d(
                vehicleNode.vehicleScale.x + 0.2,
                vehicleNode.vehicleScale.y + 0.2,
                vehicleNode.vehicleScale.z + 0.2
            )
            position: Qt.vector3d(0, vehicleNode.vehicleScale.y / 2, 0)
            opacity: 0.3

            materials: [
                PrincipledMaterial {
                    baseColor: "#ff0000"
                    roughness: 1.0
                    emissiveFactor: Qt.vector3d(1, 0, 0)
                }
            ]
        }
    }

    component PedestrianModel: Node {
        id: pedestrianNode

        property int objectId: 0
        property bool isRelevant: false
        property bool isThreat: false

        // Body
        Model {
            source: "#Cylinder"
            scale: Qt.vector3d(0.3, 0.8, 0.3)
            position: Qt.vector3d(0, 0.8, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: pedestrianNode.isThreat ? "#ff4444" :
                               (pedestrianNode.isRelevant ? "#44ff44" : "#8888ff")
                    roughness: 0.8
                }
            ]
        }

        // Head
        Model {
            source: "#Sphere"
            scale: Qt.vector3d(0.2, 0.2, 0.2)
            position: Qt.vector3d(0, 1.5, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: "#ffccaa"
                    roughness: 0.9
                }
            ]
        }
    }

    component CyclistModel: Node {
        id: cyclistNode

        property int objectId: 0
        property bool isRelevant: false
        property bool isThreat: false

        // Bike frame
        Model {
            source: "#Cube"
            scale: Qt.vector3d(0.4, 0.8, 1.8)
            position: Qt.vector3d(0, 0.6, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: cyclistNode.isThreat ? "#ff4444" : "#44aaff"
                    roughness: 0.6
                }
            ]
        }

        // Rider
        Model {
            source: "#Cylinder"
            scale: Qt.vector3d(0.25, 0.6, 0.25)
            position: Qt.vector3d(0, 1.3, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: "#ffaa44"
                    roughness: 0.8
                }
            ]
        }
    }

    component StaticObjectModel: Node {
        id: staticNode

        property int objectId: 0
        property int objectType: 7  // 7=Cone, 8=Barrier

        // Cone
        Model {
            visible: staticNode.objectType === 7
            source: "#Cone"
            scale: Qt.vector3d(0.3, 0.7, 0.3)
            position: Qt.vector3d(0, 0.35, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: "#ff6600"
                    roughness: 0.7
                    emissiveFactor: Qt.vector3d(0.3, 0.1, 0)
                }
            ]
        }

        // Barrier
        Model {
            visible: staticNode.objectType === 8
            source: "#Cube"
            scale: Qt.vector3d(0.3, 1.0, 2.0)
            position: Qt.vector3d(0, 0.5, 0)

            materials: [
                PrincipledMaterial {
                    baseColor: "#ffff00"
                    roughness: 0.8
                }
            ]
        }
    }

    // Vehicle instances
    Repeater3D {
        id: vehicleRepeater
        model: root.objectList ? filterByType(root.objectList.objects, [1, 2, 3]) : []

        delegate: VehicleModel {
            required property var modelData
            required property int index

            objectId: modelData.id
            objectType: modelData.type
            position: Qt.vector3d(modelData.position.y, 0, modelData.position.x)
            eulerRotation.y: modelData.heading
            isRelevant: modelData.isRelevant
            isThreat: modelData.isThreat
            qualityLevel: root.qualityLevel
        }
    }

    // Pedestrian instances
    Repeater3D {
        id: pedestrianRepeater
        model: root.objectList ? filterByType(root.objectList.objects, [5]) : []

        delegate: PedestrianModel {
            required property var modelData
            required property int index

            objectId: modelData.id
            position: Qt.vector3d(modelData.position.y, 0, modelData.position.x)
            eulerRotation.y: modelData.heading
            isRelevant: modelData.isRelevant
            isThreat: modelData.isThreat
        }
    }

    // Cyclist instances
    Repeater3D {
        id: cyclistRepeater
        model: root.objectList ? filterByType(root.objectList.objects, [4]) : []

        delegate: CyclistModel {
            required property var modelData
            required property int index

            objectId: modelData.id
            position: Qt.vector3d(modelData.position.y, 0, modelData.position.x)
            eulerRotation.y: modelData.heading
            isRelevant: modelData.isRelevant
            isThreat: modelData.isThreat
        }
    }

    // Static object instances (cones, barriers)
    Repeater3D {
        id: staticRepeater
        model: root.objectList ? filterByType(root.objectList.objects, [7, 8]) : []

        delegate: StaticObjectModel {
            required property var modelData
            required property int index

            objectId: modelData.id
            objectType: modelData.type
            position: Qt.vector3d(modelData.position.y, 0, modelData.position.x)
        }
    }

    // Helper function to filter objects by type
    function filterByType(objects, types) {
        if (!objects) return []
        let filtered = objects.filter(obj => types.includes(obj.type))
        return filtered.slice(0, root.maxInstances)
    }
}
