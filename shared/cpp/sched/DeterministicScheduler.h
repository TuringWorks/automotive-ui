// DeterministicScheduler.h
// Deterministic tick scheduler for safety-critical updates
// Part of: Shared Platform Layer
// Safety: Fixed update rates for deterministic behavior

#ifndef AUTOMOTIVE_DETERMINISTIC_SCHEDULER_H
#define AUTOMOTIVE_DETERMINISTIC_SCHEDULER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <functional>

namespace automotive {
namespace sched {

/**
 * @brief Scheduler statistics
 */
struct SchedulerStats {
    uint64_t tickCount{0};          ///< Total tick count
    uint64_t missedTicks{0};        ///< Number of missed/late ticks
    double avgTickDurationUs{0.0};  ///< Average tick duration in microseconds
    double maxTickDurationUs{0.0};  ///< Maximum tick duration in microseconds
    double avgJitterUs{0.0};        ///< Average timing jitter in microseconds
};

/**
 * @brief Tick callback type
 */
using TickCallback = std::function<void(uint64_t tickNumber, qint64 elapsedMs)>;

/**
 * @brief Deterministic tick scheduler
 *
 * Provides a fixed-rate tick loop for safety-critical signal processing.
 * Safety: Bounded execution, jitter monitoring, missed tick detection.
 *
 * Requirements:
 * - SR-CL-001: Speed display shall be updated at ≥10Hz
 * - Fixed 60Hz render tick, 20Hz signal state updates
 */
class DeterministicScheduler : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Standard tick rates
     */
    static constexpr int RENDER_TICK_HZ = 60;    ///< 60Hz render tick
    static constexpr int SIGNAL_TICK_HZ = 20;    ///< 20Hz signal processing

    explicit DeterministicScheduler(QObject* parent = nullptr);
    ~DeterministicScheduler() override;

    /**
     * @brief Start the scheduler
     * @param tickRateHz Tick rate in Hz
     */
    void start(int tickRateHz);

    /**
     * @brief Stop the scheduler
     */
    void stop();

    /**
     * @brief Check if scheduler is running
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Get current tick rate
     */
    int tickRateHz() const { return m_tickRateHz; }

    /**
     * @brief Get tick interval in milliseconds
     */
    int tickIntervalMs() const { return m_tickIntervalMs; }

    /**
     * @brief Get current tick number
     */
    uint64_t currentTick() const { return m_stats.tickCount; }

    /**
     * @brief Get elapsed time since start in milliseconds
     */
    qint64 elapsedMs() const;

    /**
     * @brief Get scheduler statistics
     */
    SchedulerStats statistics() const { return m_stats; }

    /**
     * @brief Register a tick callback
     * @param callback Callback to invoke on each tick
     *
     * Note: Callbacks should complete quickly to avoid jitter.
     */
    void registerTickCallback(TickCallback callback);

    /**
     * @brief Clear all tick callbacks
     */
    void clearCallbacks();

    /**
     * @brief Set jitter threshold for warning
     * @param thresholdUs Threshold in microseconds
     */
    void setJitterThreshold(double thresholdUs) { m_jitterThresholdUs = thresholdUs; }

signals:
    /**
     * @brief Emitted on each tick
     * @param tickNumber Current tick number
     * @param elapsedMs Elapsed time since start
     */
    void tick(uint64_t tickNumber, qint64 elapsedMs);

    /**
     * @brief Emitted when a tick is missed (execution took too long)
     * @param missedCount Number of missed ticks
     */
    void tickMissed(int missedCount);

    /**
     * @brief Emitted when jitter exceeds threshold
     * @param jitterUs Jitter in microseconds
     */
    void jitterExceeded(double jitterUs);

private slots:
    void onTimerTick();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    QElapsedTimer m_tickTimer;

    int m_tickRateHz{SIGNAL_TICK_HZ};
    int m_tickIntervalMs{50};
    bool m_running{false};
    double m_jitterThresholdUs{5000.0};  // 5ms default threshold

    qint64 m_lastTickTimeUs{0};
    SchedulerStats m_stats;

    QVector<TickCallback> m_callbacks;
};

} // namespace sched
} // namespace automotive

#endif // AUTOMOTIVE_DETERMINISTIC_SCHEDULER_H
