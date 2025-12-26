// AdasVisualQualityManager.h
// Benchmark-inspired Quality Manager for ADAS 3D Visualization
// Part of: Shared ADAS Platform Layer
// Safety: Quality reduction must not remove safety-critical cues
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.4, 10

#ifndef AUTOMOTIVE_ADAS_VISUAL_QUALITY_MANAGER_H
#define AUTOMOTIVE_ADAS_VISUAL_QUALITY_MANAGER_H

#include <QObject>
#include <QElapsedTimer>
#include <QMutex>

namespace automotive {
namespace adas {

/**
 * @brief Quality level presets
 */
enum class QualityLevel : uint8_t {
    Ultra = 0,      ///< Maximum quality, all effects enabled
    High,           ///< High quality, most effects enabled
    Medium,         ///< Balanced quality
    Low,            ///< Reduced quality for performance
    Minimal         ///< Minimum required for safety display
};

/**
 * @brief MSAA level options
 */
enum class MsaaLevel : uint8_t {
    None = 0,
    X2 = 2,
    X4 = 4,
    X8 = 8
};

/**
 * @brief LOD (Level of Detail) setting
 */
enum class LodLevel : uint8_t {
    High = 0,
    Medium,
    Low,
    VeryLow
};

/**
 * @brief Quality settings structure
 */
struct QualitySettings {
    // Resolution scaling (0.5 - 1.0)
    double resolutionScale{1.0};

    // Anti-aliasing
    MsaaLevel msaaLevel{MsaaLevel::X4};

    // Reflections (Eos-aligned)
    bool reflectionsEnabled{true};
    int reflectionUpdateHz{30};     ///< ReflectionProbe update rate

    // Particles (weather effects)
    bool particlesEnabled{true};
    int maxParticleCount{10000};
    double particleEmitRate{1.0};   ///< Multiplier (0.0-1.0)

    // Level of detail
    LodLevel lodLevel{LodLevel::High};

    // Post-processing effects
    bool bloomEnabled{true};
    bool fogEnabled{true};
    bool ambientOcclusionEnabled{true};

    // Animation (skeletal, etc.)
    bool skeletalAnimationEnabled{true};
    int maxAnimatedObjects{50};

    // Shadows
    bool shadowsEnabled{true};
    int shadowMapResolution{2048};

    // Instancing
    int maxInstancedObjects{500};
    int maxUniqueMaterials{32};
};

/**
 * @brief Performance metrics for quality adjustment
 */
struct PerformanceMetrics {
    double currentFps{60.0};
    double averageFps{60.0};
    double minFps{60.0};
    double gpuTimeMs{0.0};
    double cpuTimeMs{0.0};
    double frameTimeMs{16.67};
    double frameTimeVariance{0.0};

    bool thermalThrottling{false};
    double gpuTemperature{0.0};
    double memoryPressure{0.0};      ///< 0.0-1.0

    int currentParticleCount{0};
    int currentObjectCount{0};
    int droppedFrames{0};
};

/**
 * @brief ADAS Visual Quality Manager
 *
 * Dynamically adjusts 3D visualization quality based on performance.
 * Inspired by Qt Quick 3D benchmark application.
 *
 * Safety Rule (Spec Section 3.4):
 * Quality reduction must NOT remove safety-critical cues.
 * Reduce aesthetics first (reflections, extra effects), then
 * reduce non-critical detail density.
 *
 * Features:
 * - Dynamic resolution scaling
 * - MSAA level adjustment
 * - ReflectionProbe update rate control
 * - Particle budget management
 * - LOD switching
 * - Post-effect toggles
 */
class AdasVisualQualityManager : public QObject {
    Q_OBJECT

    // Current settings (for QML binding)
    Q_PROPERTY(int qualityLevel READ qualityLevelInt NOTIFY qualityChanged)
    Q_PROPERTY(double resolutionScale READ resolutionScale NOTIFY settingsChanged)
    Q_PROPERTY(int msaaLevel READ msaaLevelInt NOTIFY settingsChanged)
    Q_PROPERTY(bool reflectionsEnabled READ reflectionsEnabled NOTIFY settingsChanged)
    Q_PROPERTY(bool particlesEnabled READ particlesEnabled NOTIFY settingsChanged)
    Q_PROPERTY(int maxParticleCount READ maxParticleCount NOTIFY settingsChanged)
    Q_PROPERTY(int lodLevel READ lodLevelInt NOTIFY settingsChanged)
    Q_PROPERTY(bool bloomEnabled READ bloomEnabled NOTIFY settingsChanged)
    Q_PROPERTY(bool shadowsEnabled READ shadowsEnabled NOTIFY settingsChanged)

    // Performance metrics
    Q_PROPERTY(double currentFps READ currentFps NOTIFY metricsChanged)
    Q_PROPERTY(double averageFps READ averageFps NOTIFY metricsChanged)
    Q_PROPERTY(bool thermalThrottling READ thermalThrottling NOTIFY metricsChanged)

    // Mode
    Q_PROPERTY(bool autoAdjustEnabled READ autoAdjustEnabled
               WRITE setAutoAdjustEnabled NOTIFY autoAdjustChanged)

public:
    explicit AdasVisualQualityManager(QObject* parent = nullptr);
    ~AdasVisualQualityManager() override;

    // Quality level
    QualityLevel qualityLevel() const;
    int qualityLevelInt() const { return static_cast<int>(qualityLevel()); }
    void setQualityLevel(QualityLevel level);
    Q_INVOKABLE void setQualityLevelInt(int level);

    // Current settings access
    QualitySettings currentSettings() const;
    double resolutionScale() const;
    MsaaLevel msaaLevel() const;
    int msaaLevelInt() const { return static_cast<int>(msaaLevel()); }
    bool reflectionsEnabled() const;
    bool particlesEnabled() const;
    int maxParticleCount() const;
    LodLevel lodLevel() const;
    int lodLevelInt() const { return static_cast<int>(lodLevel()); }
    bool bloomEnabled() const;
    bool shadowsEnabled() const;

    // Performance metrics
    PerformanceMetrics metrics() const;
    double currentFps() const;
    double averageFps() const;
    bool thermalThrottling() const;

    // Auto-adjustment
    bool autoAdjustEnabled() const;
    void setAutoAdjustEnabled(bool enabled);

    /**
     * @brief Update performance metrics (call each frame)
     */
    void updateMetrics(double frameTimeMs, double gpuTimeMs = 0.0, double cpuTimeMs = 0.0);

    /**
     * @brief Report current scene complexity
     */
    void reportSceneComplexity(int particleCount, int objectCount);

    /**
     * @brief Report thermal state
     */
    void reportThermalState(bool throttling, double gpuTemp = 0.0);

    /**
     * @brief Report memory pressure
     */
    void reportMemoryPressure(double pressure);

    /**
     * @brief Force quality update check
     */
    void checkQuality();

    /**
     * @brief Get settings for a specific quality level
     */
    static QualitySettings settingsForLevel(QualityLevel level);

    // Target FPS (default 60)
    void setTargetFps(double fps);
    double targetFps() const;

    // Thresholds
    static constexpr double FPS_CRITICAL_THRESHOLD = 30.0;
    static constexpr double FPS_WARNING_THRESHOLD = 45.0;
    static constexpr double FPS_TARGET_THRESHOLD = 55.0;
    static constexpr double FRAME_TIME_VARIANCE_THRESHOLD = 5.0;  // ms
    static constexpr int QUALITY_ADJUST_COOLDOWN_MS = 2000;

signals:
    void qualityChanged(QualityLevel level);
    void settingsChanged();
    void metricsChanged();
    void autoAdjustChanged(bool enabled);

    // Safety signals
    void performanceCritical();  ///< FPS dropped to critical level
    void performanceRecovered(); ///< FPS recovered to acceptable level

private:
    void applySettings(const QualitySettings& settings);
    void adjustQualityDown();
    void adjustQualityUp();
    void updateAverageFps(double frameTimeMs);
    qint64 currentMonotonicTimeMs() const;

    mutable QMutex m_mutex;

    QualityLevel m_qualityLevel{QualityLevel::High};
    QualitySettings m_settings;
    PerformanceMetrics m_metrics;

    bool m_autoAdjust{true};
    double m_targetFps{60.0};

    // Averaging
    double m_fpsAccumulator{0.0};
    int m_fpsFrameCount{0};
    static constexpr int FPS_AVERAGE_FRAMES = 30;

    // Cooldown to prevent oscillation
    qint64 m_lastQualityAdjustMs{0};
    bool m_wasCritical{false};

    QElapsedTimer m_monotonicTimer;
};

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::QualityLevel)
Q_DECLARE_METATYPE(automotive::adas::MsaaLevel)
Q_DECLARE_METATYPE(automotive::adas::LodLevel)

#endif // AUTOMOTIVE_ADAS_VISUAL_QUALITY_MANAGER_H
