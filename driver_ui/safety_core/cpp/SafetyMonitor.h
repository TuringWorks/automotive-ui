// SafetyMonitor.h
// Safety monitoring and watchdog
// Safety: Monitors system health and triggers safety responses

#ifndef AUTOMOTIVE_SAFETY_MONITOR_H
#define AUTOMOTIVE_SAFETY_MONITOR_H

#include <QObject>
#include <QElapsedTimer>
#include "signal/SignalHub.h"
#include "sched/DeterministicScheduler.h"

namespace automotive {
namespace driver {

/**
 * @brief Safety monitor state
 */
enum class MonitorState : uint8_t {
    Ok = 0,
    Warning,
    Fault
};

/**
 * @brief Safety monitor for cluster
 *
 * Monitors:
 * - Signal freshness and validity
 * - Scheduler tick regularity
 * - Frame rate (render health)
 * - Memory usage bounds
 */
class SafetyMonitor : public QObject {
    Q_OBJECT

    Q_PROPERTY(MonitorState state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool isHealthy READ isHealthy NOTIFY isHealthyChanged)
    Q_PROPERTY(double frameRate READ frameRate NOTIFY frameRateChanged)
    Q_PROPERTY(int missedFrames READ missedFrames NOTIFY missedFramesChanged)

public:
    // Thresholds
    static constexpr int MIN_FRAME_RATE = 30;
    static constexpr int TARGET_FRAME_RATE = 60;
    static constexpr int MAX_MISSED_FRAMES = 5;
    static constexpr double MAX_JITTER_MS = 10.0;

    explicit SafetyMonitor(signal::SignalHub* signalHub,
                           sched::DeterministicScheduler* scheduler,
                           QObject* parent = nullptr);
    ~SafetyMonitor() override;

    MonitorState state() const { return m_state; }
    bool isHealthy() const { return m_state == MonitorState::Ok; }
    double frameRate() const { return m_frameRate; }
    int missedFrames() const { return m_missedFrames; }

    /**
     * @brief Record frame render (call from QML)
     */
    Q_INVOKABLE void recordFrame();

    /**
     * @brief Process monitoring tick
     */
    void processTick(qint64 currentTimeMs);

    /**
     * @brief Get diagnostic report
     */
    Q_INVOKABLE QVariantMap getDiagnostics() const;

signals:
    void stateChanged(MonitorState state);
    void isHealthyChanged(bool healthy);
    void frameRateChanged(double rate);
    void missedFramesChanged(int count);
    void healthWarning(const QString& message);
    void healthFault(const QString& message);

private slots:
    void onTickMissed(int count);
    void onJitterExceeded(double jitterUs);

private:
    void updateState();

    signal::SignalHub* m_signalHub{nullptr};
    sched::DeterministicScheduler* m_scheduler{nullptr};

    MonitorState m_state{MonitorState::Ok};
    double m_frameRate{0.0};
    int m_missedFrames{0};
    int m_missedTicks{0};
    double m_maxJitterUs{0.0};

    // Frame counting
    QElapsedTimer m_frameTimer;
    int m_frameCount{0};
    qint64 m_lastFrameCheckMs{0};
};

} // namespace driver
} // namespace automotive

Q_DECLARE_METATYPE(automotive::driver::MonitorState)

#endif // AUTOMOTIVE_SAFETY_MONITOR_H
