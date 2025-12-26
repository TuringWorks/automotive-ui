// AdasScene3D.qml
// Main 3D Scene for ADAS Visualization
// Part of: Shared ADAS 3D Components
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.3, 10

import QtQuick
import QtQuick3D
import QtQuick3D.Particles3D

/**
 * Main ADAS 3D visualization scene.
 *
 * Features:
 * - Road/lane visualization
 * - Object instancing for vehicles
 * - Environment presets (dawn/day/dusk/night)
 * - Weather particles
 * - Reflection probes (quality-scaled)
 *
 * Safety: This is visualization only.
 * All ADAS logic is in C++ (AdasStateService).
 */
Item {
    id: root

    // Quality settings (from AdasVisualQualityManager)
    property int qualityLevel: 1  // 0=Ultra, 1=High, 2=Medium, 3=Low, 4=Minimal
    property real resolutionScale: 1.0
    property int msaaLevel: 4
    property bool reflectionsEnabled: true
    property bool particlesEnabled: true
    property int maxParticleCount: 10000
    property bool shadowsEnabled: true
    property bool bloomEnabled: true

    // Environment preset (from EnvironmentPresets)
    property int environmentPreset: 1  // 0=Dawn, 1=Day, 2=Dusk, 3=Night

    // Perception data binding points
    property var laneModel: null
    property var leadVehicle: null
    property var objectList: null
    property var corridorData: null

    // Weather condition
    property int weatherCondition: 0  // 0=Clear, 1=Rain, 2=Snow, 3=Fog

    // Camera controls
    property real cameraDistance: 15.0
    property real cameraHeight: 8.0
    property real cameraPitch: -25.0

    // Signals for interaction
    signal objectClicked(int objectId)
    signal sceneReady()

    View3D {
        id: view3d
        anchors.fill: parent

        // Render settings based on quality
        renderMode: View3D.Offscreen

        environment: sceneEnvironment

        SceneEnvironment {
            id: sceneEnvironment

            // Anti-aliasing based on quality
            antialiasingMode: root.msaaLevel > 0 ?
                SceneEnvironment.MSAA : SceneEnvironment.NoAA
            antialiasingQuality: {
                if (root.msaaLevel >= 8) return SceneEnvironment.VeryHigh
                if (root.msaaLevel >= 4) return SceneEnvironment.High
                if (root.msaaLevel >= 2) return SceneEnvironment.Medium
                return SceneEnvironment.Medium
            }

            // Background based on environment preset
            backgroundMode: SceneEnvironment.Color
            clearColor: environmentColors.backgroundColor

            // Effects based on quality
            aoEnabled: root.qualityLevel <= 1
            aoStrength: 50
            aoDistance: 5
            aoSoftness: 50

            // Fog for atmosphere
            fog: Fog {
                enabled: root.qualityLevel <= 2 && root.weatherCondition !== 0
                color: environmentColors.fogColor
                density: root.weatherCondition === 3 ? 0.02 : 0.005  // Heavy fog vs light
                depthEnabled: true
                depthNear: 10
                depthFar: 200
            }

            // Tonemapping
            tonemapMode: SceneEnvironment.TonemapModeLinear
        }

        // Main camera
        PerspectiveCamera {
            id: mainCamera
            position: Qt.vector3d(0, root.cameraHeight, -root.cameraDistance)
            eulerRotation.x: root.cameraPitch
            clipNear: 0.1
            clipFar: 500
            fieldOfView: 60
        }

        // Environment lighting (changes with preset)
        DirectionalLight {
            id: sunLight
            eulerRotation: environmentColors.sunRotation
            color: environmentColors.sunColor
            brightness: environmentColors.sunBrightness
            castsShadow: root.shadowsEnabled && root.qualityLevel <= 2
            shadowFactor: 50
            shadowMapQuality: root.qualityLevel <= 1 ?
                Light.ShadowMapQualityHigh : Light.ShadowMapQualityMedium
        }

        // Ambient light
        DirectionalLight {
            id: ambientLight
            eulerRotation.x: -90
            color: environmentColors.ambientColor
            brightness: environmentColors.ambientBrightness
            castsShadow: false
        }

        // Reflection probe (quality-scaled)
        ReflectionProbe {
            id: reflectionProbe
            visible: root.reflectionsEnabled && root.qualityLevel <= 2
            position: Qt.vector3d(0, 2, 0)
            boxSize: Qt.vector3d(100, 20, 200)
            quality: root.qualityLevel <= 1 ?
                ReflectionProbe.High : ReflectionProbe.Medium
            refreshMode: root.qualityLevel <= 0 ?
                ReflectionProbe.EveryFrame : ReflectionProbe.FirstFrame
            timeSlicing: ReflectionProbe.AllFacesAtOnce
        }

        // Ground plane
        Model {
            id: groundPlane
            source: "#Rectangle"
            scale: Qt.vector3d(100, 100, 1)
            eulerRotation.x: -90
            position: Qt.vector3d(0, -0.1, 50)
            materials: [
                PrincipledMaterial {
                    baseColor: environmentColors.groundColor
                    roughness: 0.8
                    metalness: 0.0
                }
            ]
        }

        // Road surface
        RoadVisualization {
            id: roadViz
            laneModel: root.laneModel
            corridorData: root.corridorData
            qualityLevel: root.qualityLevel
            environmentPreset: root.environmentPreset
        }

        // Lead vehicle marker
        LeadVehicleMarker {
            id: leadMarker
            visible: root.leadVehicle !== null && root.leadVehicle.detected
            distance: root.leadVehicle ? root.leadVehicle.distanceM : 0
            lateralOffset: root.leadVehicle ? root.leadVehicle.lateralOffsetM : 0
            relativeSpeed: root.leadVehicle ? root.leadVehicle.relativeSpeedMps : 0
            isClosing: root.leadVehicle ? root.leadVehicle.isClosing : false
        }

        // Object instances (vehicles, pedestrians)
        ObjectInstances {
            id: objectInstances
            objectList: root.objectList
            qualityLevel: root.qualityLevel
            onObjectClicked: (id) => root.objectClicked(id)
        }

        // Weather particles
        WeatherParticles {
            id: weatherParticles
            visible: root.particlesEnabled && root.weatherCondition !== 0
            weatherCondition: root.weatherCondition
            maxParticleCount: root.maxParticleCount
            qualityLevel: root.qualityLevel
        }
    }

    // Environment color definitions based on preset
    QtObject {
        id: environmentColors

        property color backgroundColor: {
            switch (root.environmentPreset) {
                case 0: return "#2a3040"  // Dawn
                case 1: return "#87CEEB"  // Day
                case 2: return "#4a3050"  // Dusk
                case 3: return "#0a0a15"  // Night
                default: return "#87CEEB"
            }
        }

        property color sunColor: {
            switch (root.environmentPreset) {
                case 0: return "#ffaa77"  // Dawn - warm orange
                case 1: return "#fffaf0"  // Day - bright white
                case 2: return "#ff7744"  // Dusk - orange-red
                case 3: return "#334466"  // Night - blue moonlight
                default: return "#ffffff"
            }
        }

        property real sunBrightness: {
            switch (root.environmentPreset) {
                case 0: return 0.7   // Dawn
                case 1: return 1.0   // Day
                case 2: return 0.6   // Dusk
                case 3: return 0.2   // Night
                default: return 1.0
            }
        }

        property vector3d sunRotation: {
            switch (root.environmentPreset) {
                case 0: return Qt.vector3d(-20, 90, 0)   // Dawn - low east
                case 1: return Qt.vector3d(-60, 0, 0)   // Day - high
                case 2: return Qt.vector3d(-15, -90, 0) // Dusk - low west
                case 3: return Qt.vector3d(-30, 45, 0)  // Night - moon angle
                default: return Qt.vector3d(-45, 0, 0)
            }
        }

        property color ambientColor: {
            switch (root.environmentPreset) {
                case 0: return "#6080a0"
                case 1: return "#c0d0e0"
                case 2: return "#806040"
                case 3: return "#202030"
                default: return "#808080"
            }
        }

        property real ambientBrightness: {
            switch (root.environmentPreset) {
                case 0: return 0.3
                case 1: return 0.4
                case 2: return 0.25
                case 3: return 0.1
                default: return 0.3
            }
        }

        property color groundColor: {
            switch (root.environmentPreset) {
                case 0: return "#303838"
                case 1: return "#404840"
                case 2: return "#383030"
                case 3: return "#181818"
                default: return "#404040"
            }
        }

        property color fogColor: {
            switch (root.environmentPreset) {
                case 0: return "#506070"
                case 1: return "#c0c8d0"
                case 2: return "#605040"
                case 3: return "#101520"
                default: return "#808080"
            }
        }
    }

    // Performance monitoring
    property int frameCount: 0
    property real lastFrameTime: 0

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            // Report FPS to quality manager via C++ binding
            root.frameCount = 0
        }
    }

    Component.onCompleted: {
        root.sceneReady()
    }
}
