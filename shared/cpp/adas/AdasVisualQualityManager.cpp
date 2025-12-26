// AdasVisualQualityManager.cpp
// Benchmark-inspired Quality Manager Implementation
// Safety: Preserves safety-critical cues during quality reduction

#include "AdasVisualQualityManager.h"
#include <QMutexLocker>
#include <cmath>

namespace automotive {
namespace adas {

AdasVisualQualityManager::AdasVisualQualityManager(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
    m_settings = settingsForLevel(m_qualityLevel);
}

AdasVisualQualityManager::~AdasVisualQualityManager() = default;

qint64 AdasVisualQualityManager::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

QualityLevel AdasVisualQualityManager::qualityLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_qualityLevel;
}

void AdasVisualQualityManager::setQualityLevel(QualityLevel level)
{
    {
        QMutexLocker locker(&m_mutex);
        if (m_qualityLevel == level) {
            return;
        }
        m_qualityLevel = level;
        m_settings = settingsForLevel(level);
        m_lastQualityAdjustMs = currentMonotonicTimeMs();
    }

    emit qualityChanged(level);
    emit settingsChanged();
}

void AdasVisualQualityManager::setQualityLevelInt(int level)
{
    setQualityLevel(static_cast<QualityLevel>(level));
}

QualitySettings AdasVisualQualityManager::currentSettings() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings;
}

double AdasVisualQualityManager::resolutionScale() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.resolutionScale;
}

MsaaLevel AdasVisualQualityManager::msaaLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.msaaLevel;
}

bool AdasVisualQualityManager::reflectionsEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.reflectionsEnabled;
}

bool AdasVisualQualityManager::particlesEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.particlesEnabled;
}

int AdasVisualQualityManager::maxParticleCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.maxParticleCount;
}

LodLevel AdasVisualQualityManager::lodLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.lodLevel;
}

bool AdasVisualQualityManager::bloomEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.bloomEnabled;
}

bool AdasVisualQualityManager::shadowsEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_settings.shadowsEnabled;
}

PerformanceMetrics AdasVisualQualityManager::metrics() const
{
    QMutexLocker locker(&m_mutex);
    return m_metrics;
}

double AdasVisualQualityManager::currentFps() const
{
    QMutexLocker locker(&m_mutex);
    return m_metrics.currentFps;
}

double AdasVisualQualityManager::averageFps() const
{
    QMutexLocker locker(&m_mutex);
    return m_metrics.averageFps;
}

bool AdasVisualQualityManager::thermalThrottling() const
{
    QMutexLocker locker(&m_mutex);
    return m_metrics.thermalThrottling;
}

bool AdasVisualQualityManager::autoAdjustEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_autoAdjust;
}

void AdasVisualQualityManager::setAutoAdjustEnabled(bool enabled)
{
    {
        QMutexLocker locker(&m_mutex);
        if (m_autoAdjust == enabled) {
            return;
        }
        m_autoAdjust = enabled;
    }
    emit autoAdjustChanged(enabled);
}

void AdasVisualQualityManager::updateMetrics(double frameTimeMs, double gpuTimeMs, double cpuTimeMs)
{
    bool shouldEmit = false;
    bool wasCritical = false;
    bool isCritical = false;

    {
        QMutexLocker locker(&m_mutex);

        m_metrics.frameTimeMs = frameTimeMs;
        m_metrics.gpuTimeMs = gpuTimeMs;
        m_metrics.cpuTimeMs = cpuTimeMs;
        m_metrics.currentFps = (frameTimeMs > 0) ? (1000.0 / frameTimeMs) : 0.0;

        updateAverageFps(frameTimeMs);

        if (m_metrics.currentFps < FPS_CRITICAL_THRESHOLD) {
            m_metrics.droppedFrames++;
        }

        wasCritical = m_wasCritical;
        isCritical = m_metrics.averageFps < FPS_CRITICAL_THRESHOLD;
        m_wasCritical = isCritical;

        shouldEmit = true;
    }

    if (shouldEmit) {
        emit metricsChanged();
    }

    if (isCritical && !wasCritical) {
        emit performanceCritical();
    } else if (!isCritical && wasCritical) {
        emit performanceRecovered();
    }

    // Auto-adjust quality if enabled
    QMutexLocker locker(&m_mutex);
    if (m_autoAdjust) {
        locker.unlock();
        checkQuality();
    }
}

void AdasVisualQualityManager::reportSceneComplexity(int particleCount, int objectCount)
{
    QMutexLocker locker(&m_mutex);
    m_metrics.currentParticleCount = particleCount;
    m_metrics.currentObjectCount = objectCount;
}

void AdasVisualQualityManager::reportThermalState(bool throttling, double gpuTemp)
{
    {
        QMutexLocker locker(&m_mutex);
        m_metrics.thermalThrottling = throttling;
        m_metrics.gpuTemperature = gpuTemp;
    }

    if (throttling) {
        // Force quality reduction on thermal throttling
        adjustQualityDown();
    }

    emit metricsChanged();
}

void AdasVisualQualityManager::reportMemoryPressure(double pressure)
{
    {
        QMutexLocker locker(&m_mutex);
        m_metrics.memoryPressure = pressure;
    }

    if (pressure > 0.8) {
        adjustQualityDown();
    }

    emit metricsChanged();
}

void AdasVisualQualityManager::checkQuality()
{
    QMutexLocker locker(&m_mutex);

    const qint64 currentTime = currentMonotonicTimeMs();
    if ((currentTime - m_lastQualityAdjustMs) < QUALITY_ADJUST_COOLDOWN_MS) {
        return; // Still in cooldown
    }

    if (m_metrics.averageFps < FPS_WARNING_THRESHOLD ||
        m_metrics.thermalThrottling ||
        m_metrics.memoryPressure > 0.7) {
        locker.unlock();
        adjustQualityDown();
    } else if (m_metrics.averageFps > m_targetFps &&
               m_qualityLevel != QualityLevel::Ultra &&
               !m_metrics.thermalThrottling) {
        locker.unlock();
        adjustQualityUp();
    }
}

void AdasVisualQualityManager::setTargetFps(double fps)
{
    QMutexLocker locker(&m_mutex);
    m_targetFps = fps;
}

double AdasVisualQualityManager::targetFps() const
{
    QMutexLocker locker(&m_mutex);
    return m_targetFps;
}

QualitySettings AdasVisualQualityManager::settingsForLevel(QualityLevel level)
{
    QualitySettings settings;

    switch (level) {
        case QualityLevel::Ultra:
            settings.resolutionScale = 1.0;
            settings.msaaLevel = MsaaLevel::X8;
            settings.reflectionsEnabled = true;
            settings.reflectionUpdateHz = 60;
            settings.particlesEnabled = true;
            settings.maxParticleCount = 20000;
            settings.particleEmitRate = 1.0;
            settings.lodLevel = LodLevel::High;
            settings.bloomEnabled = true;
            settings.fogEnabled = true;
            settings.ambientOcclusionEnabled = true;
            settings.skeletalAnimationEnabled = true;
            settings.maxAnimatedObjects = 100;
            settings.shadowsEnabled = true;
            settings.shadowMapResolution = 4096;
            settings.maxInstancedObjects = 1000;
            settings.maxUniqueMaterials = 64;
            break;

        case QualityLevel::High:
            settings.resolutionScale = 1.0;
            settings.msaaLevel = MsaaLevel::X4;
            settings.reflectionsEnabled = true;
            settings.reflectionUpdateHz = 30;
            settings.particlesEnabled = true;
            settings.maxParticleCount = 10000;
            settings.particleEmitRate = 1.0;
            settings.lodLevel = LodLevel::High;
            settings.bloomEnabled = true;
            settings.fogEnabled = true;
            settings.ambientOcclusionEnabled = true;
            settings.skeletalAnimationEnabled = true;
            settings.maxAnimatedObjects = 50;
            settings.shadowsEnabled = true;
            settings.shadowMapResolution = 2048;
            settings.maxInstancedObjects = 500;
            settings.maxUniqueMaterials = 32;
            break;

        case QualityLevel::Medium:
            settings.resolutionScale = 0.85;
            settings.msaaLevel = MsaaLevel::X2;
            settings.reflectionsEnabled = true;
            settings.reflectionUpdateHz = 15;
            settings.particlesEnabled = true;
            settings.maxParticleCount = 5000;
            settings.particleEmitRate = 0.7;
            settings.lodLevel = LodLevel::Medium;
            settings.bloomEnabled = true;
            settings.fogEnabled = true;
            settings.ambientOcclusionEnabled = false;
            settings.skeletalAnimationEnabled = true;
            settings.maxAnimatedObjects = 20;
            settings.shadowsEnabled = true;
            settings.shadowMapResolution = 1024;
            settings.maxInstancedObjects = 300;
            settings.maxUniqueMaterials = 16;
            break;

        case QualityLevel::Low:
            settings.resolutionScale = 0.75;
            settings.msaaLevel = MsaaLevel::None;
            settings.reflectionsEnabled = false;  // Disable reflections first
            settings.reflectionUpdateHz = 0;
            settings.particlesEnabled = true;     // Keep particles for weather cues
            settings.maxParticleCount = 2000;
            settings.particleEmitRate = 0.5;
            settings.lodLevel = LodLevel::Low;
            settings.bloomEnabled = false;
            settings.fogEnabled = true;           // Keep fog for visibility cue
            settings.ambientOcclusionEnabled = false;
            settings.skeletalAnimationEnabled = false;
            settings.maxAnimatedObjects = 0;
            settings.shadowsEnabled = false;
            settings.shadowMapResolution = 512;
            settings.maxInstancedObjects = 150;
            settings.maxUniqueMaterials = 8;
            break;

        case QualityLevel::Minimal:
            // Safety: Maintain minimum required for safe display
            settings.resolutionScale = 0.7;
            settings.msaaLevel = MsaaLevel::None;
            settings.reflectionsEnabled = false;
            settings.reflectionUpdateHz = 0;
            settings.particlesEnabled = false;    // Disable particles
            settings.maxParticleCount = 0;
            settings.particleEmitRate = 0.0;
            settings.lodLevel = LodLevel::VeryLow;
            settings.bloomEnabled = false;
            settings.fogEnabled = false;
            settings.ambientOcclusionEnabled = false;
            settings.skeletalAnimationEnabled = false;
            settings.maxAnimatedObjects = 0;
            settings.shadowsEnabled = false;
            settings.shadowMapResolution = 256;
            settings.maxInstancedObjects = 50;
            settings.maxUniqueMaterials = 4;
            break;
    }

    return settings;
}

void AdasVisualQualityManager::applySettings(const QualitySettings& settings)
{
    {
        QMutexLocker locker(&m_mutex);
        m_settings = settings;
        m_lastQualityAdjustMs = currentMonotonicTimeMs();
    }
    emit settingsChanged();
}

void AdasVisualQualityManager::adjustQualityDown()
{
    QualityLevel currentLevel = qualityLevel();
    if (currentLevel == QualityLevel::Minimal) {
        return; // Already at minimum
    }

    QualityLevel newLevel = static_cast<QualityLevel>(
        static_cast<int>(currentLevel) + 1);
    setQualityLevel(newLevel);
}

void AdasVisualQualityManager::adjustQualityUp()
{
    QualityLevel currentLevel = qualityLevel();
    if (currentLevel == QualityLevel::Ultra) {
        return; // Already at maximum
    }

    QualityLevel newLevel = static_cast<QualityLevel>(
        static_cast<int>(currentLevel) - 1);
    setQualityLevel(newLevel);
}

void AdasVisualQualityManager::updateAverageFps(double frameTimeMs)
{
    // Rolling average over FPS_AVERAGE_FRAMES
    m_fpsAccumulator += (frameTimeMs > 0) ? (1000.0 / frameTimeMs) : 0.0;
    m_fpsFrameCount++;

    if (m_fpsFrameCount >= FPS_AVERAGE_FRAMES) {
        m_metrics.averageFps = m_fpsAccumulator / m_fpsFrameCount;
        m_fpsAccumulator = 0.0;
        m_fpsFrameCount = 0;

        // Track min FPS
        m_metrics.minFps = std::min(m_metrics.minFps, m_metrics.averageFps);
    }
}

} // namespace adas
} // namespace automotive
