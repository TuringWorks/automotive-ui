// WeatherParticles.qml
// Weather Particle Effects for ADAS Visualization
// Part of: Shared ADAS 3D Components
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 10.3

import QtQuick
import QtQuick3D
import QtQuick3D.Particles3D

/**
 * Weather particle effects (rain/snow/mist).
 *
 * Purpose:
 * - Situational awareness channel
 * - ODD/degraded-mode visual cue
 *
 * Safety: SOTIF-UI-010
 * Particles are informational only - not primary safety signals.
 * Driver UI still shows explicit text/icon degraded-mode cues.
 */
Node {
    id: root

    // Weather condition (from WeatherCondition enum)
    // 0=Clear, 1=Cloudy, 2=LightRain, 3=HeavyRain, 4=LightSnow, 5=HeavySnow, 6=Fog, 7=Mist
    property int weatherCondition: 0

    // Quality settings (from AdasVisualQualityManager)
    property int maxParticleCount: 10000
    property int qualityLevel: 1

    // Derived visibility
    property bool isRaining: weatherCondition === 2 || weatherCondition === 3
    property bool isSnowing: weatherCondition === 4 || weatherCondition === 5
    property bool isFoggy: weatherCondition === 6 || weatherCondition === 7
    property bool isHeavy: weatherCondition === 3 || weatherCondition === 5

    // Calculate particle count based on quality
    property int effectiveParticleCount: {
        let base = root.maxParticleCount
        switch (root.qualityLevel) {
            case 0: return base         // Ultra
            case 1: return base * 0.8   // High
            case 2: return base * 0.5   // Medium
            case 3: return base * 0.3   // Low
            default: return 0           // Minimal - no particles
        }
    }

    // Particle system for rain
    ParticleSystem3D {
        id: rainSystem
        visible: root.isRaining
        running: root.isRaining

        // Rain particles fall from above
        SpriteParticle3D {
            id: rainParticle
            sprite: Texture {
                source: "qrc:/assets/particles/raindrop.png"
            }
            maxAmount: root.effectiveParticleCount
            color: "#80ffffff"
            colorVariation: Qt.vector4d(0, 0, 0.1, 0.2)
            billboard: true
            blendMode: SpriteParticle3D.SourceOver
        }

        ParticleEmitter3D {
            id: rainEmitter
            particle: rainParticle
            enabled: root.isRaining

            // Emit from a large area above
            position: Qt.vector3d(0, 20, 50)
            shape: ParticleShape3D {
                type: ParticleShape3D.Cube
                extents: Qt.vector3d(30, 1, 100)
            }

            emitRate: root.isHeavy ? 2000 : 800
            lifeSpan: 2000
            lifeSpanVariation: 500

            particleScale: root.isHeavy ? 0.08 : 0.05
            particleScaleVariation: 0.02

            velocity: VectorDirection3D {
                direction: Qt.vector3d(0, -15, 0)   // Fall speed
                directionVariation: Qt.vector3d(1, 2, 1)
            }
        }

        // Gravity for rain
        Gravity3D {
            direction: Qt.vector3d(0, -1, 0)
            magnitude: 200
        }
    }

    // Particle system for snow
    ParticleSystem3D {
        id: snowSystem
        visible: root.isSnowing
        running: root.isSnowing

        SpriteParticle3D {
            id: snowParticle
            sprite: Texture {
                source: "qrc:/assets/particles/snowflake.png"
            }
            maxAmount: root.effectiveParticleCount * 0.7  // Snow needs fewer particles
            color: "#ffffff"
            colorVariation: Qt.vector4d(0, 0, 0, 0.2)
            billboard: true
            blendMode: SpriteParticle3D.SourceOver
        }

        ParticleEmitter3D {
            id: snowEmitter
            particle: snowParticle
            enabled: root.isSnowing

            position: Qt.vector3d(0, 15, 50)
            shape: ParticleShape3D {
                type: ParticleShape3D.Cube
                extents: Qt.vector3d(40, 1, 120)
            }

            emitRate: root.isHeavy ? 800 : 300
            lifeSpan: 5000
            lifeSpanVariation: 1000

            particleScale: root.isHeavy ? 0.15 : 0.1
            particleScaleVariation: 0.05

            velocity: VectorDirection3D {
                direction: Qt.vector3d(0, -3, 0)   // Slow fall
                directionVariation: Qt.vector3d(2, 1, 2)
            }

            particleRotation: Qt.vector3d(0, 0, 0)
            particleRotationVariation: Qt.vector3d(180, 180, 180)
            particleRotationVelocity: Qt.vector3d(0, 0, 50)
            particleRotationVelocityVariation: Qt.vector3d(20, 20, 30)
        }

        // Gentle gravity for snow
        Gravity3D {
            direction: Qt.vector3d(0, -1, 0)
            magnitude: 30
        }

        // Wind effect for snow
        Wander3D {
            uniqueAmount: Qt.vector3d(20, 5, 20)
            uniqueAmountVariation: 0.5
            uniquePace: Qt.vector3d(0.5, 0.2, 0.5)
            uniquePaceVariation: 0.3
        }
    }

    // Fog/mist effect (volumetric-like using many small particles)
    ParticleSystem3D {
        id: fogSystem
        visible: root.isFoggy && root.qualityLevel <= 2
        running: root.isFoggy

        SpriteParticle3D {
            id: fogParticle
            sprite: Texture {
                source: "qrc:/assets/particles/fog_puff.png"
            }
            maxAmount: root.effectiveParticleCount * 0.3
            color: weatherCondition === 6 ? "#40808080" : "#30a0a0a0"  // Fog vs mist
            billboard: true
            blendMode: SpriteParticle3D.SourceOver
        }

        ParticleEmitter3D {
            id: fogEmitter
            particle: fogParticle
            enabled: root.isFoggy

            position: Qt.vector3d(0, 2, 60)
            shape: ParticleShape3D {
                type: ParticleShape3D.Cube
                extents: Qt.vector3d(50, 8, 150)
            }

            emitRate: weatherCondition === 6 ? 50 : 30
            lifeSpan: 8000
            lifeSpanVariation: 2000

            particleScale: 5.0
            particleScaleVariation: 2.0

            velocity: VectorDirection3D {
                direction: Qt.vector3d(0, 0.2, -1)   // Slight drift
                directionVariation: Qt.vector3d(1, 0.5, 1)
            }
        }

        Wander3D {
            uniqueAmount: Qt.vector3d(10, 2, 5)
            uniquePace: Qt.vector3d(0.1, 0.05, 0.1)
        }
    }

    // Splash effects for rain hitting road (high quality only)
    ParticleSystem3D {
        id: splashSystem
        visible: root.isRaining && root.qualityLevel <= 1
        running: root.isRaining && root.qualityLevel <= 1

        SpriteParticle3D {
            id: splashParticle
            sprite: Texture {
                source: "qrc:/assets/particles/splash.png"
            }
            maxAmount: 500
            color: "#60ffffff"
            billboard: true
            fadeInDuration: 50
            fadeOutDuration: 200
        }

        ParticleEmitter3D {
            particle: splashParticle

            position: Qt.vector3d(0, 0.1, 30)
            shape: ParticleShape3D {
                type: ParticleShape3D.Cube
                extents: Qt.vector3d(10, 0.1, 60)
            }

            emitRate: root.isHeavy ? 200 : 80
            lifeSpan: 300

            particleScale: 0.1
            particleScaleVariation: 0.05

            velocity: VectorDirection3D {
                direction: Qt.vector3d(0, 2, 0)
                directionVariation: Qt.vector3d(1, 1, 1)
            }
        }
    }
}
