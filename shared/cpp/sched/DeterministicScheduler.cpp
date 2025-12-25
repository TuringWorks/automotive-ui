// DeterministicScheduler.cpp
// Deterministic tick scheduler implementation

#include "sched/DeterministicScheduler.h"
#include <QDebug>
#include <cmath>

namespace automotive {
namespace sched {

DeterministicScheduler::DeterministicScheduler(QObject* parent)
    : QObject(parent)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &DeterministicScheduler::onTimerTick);
}

DeterministicScheduler::~DeterministicScheduler()
{
    stop();
}

void DeterministicScheduler::start(int tickRateHz)
{
    if (m_running) {
        stop();
    }

    m_tickRateHz = tickRateHz;
    m_tickIntervalMs = 1000 / tickRateHz;

    // Reset statistics
    m_stats = SchedulerStats{};
    m_lastTickTimeUs = 0;

    m_elapsedTimer.start();
    m_tickTimer.start();

    m_timer.start(m_tickIntervalMs);
    m_running = true;

    qDebug() << "DeterministicScheduler: Started at" << tickRateHz << "Hz"
             << "(" << m_tickIntervalMs << "ms interval)";
}

void DeterministicScheduler::stop()
{
    if (!m_running) {
        return;
    }

    m_timer.stop();
    m_running = false;

    qDebug() << "DeterministicScheduler: Stopped after" << m_stats.tickCount << "ticks"
             << "(" << m_stats.missedTicks << "missed)";
}

qint64 DeterministicScheduler::elapsedMs() const
{
    return m_elapsedTimer.isValid() ? m_elapsedTimer.elapsed() : 0;
}

void DeterministicScheduler::registerTickCallback(TickCallback callback)
{
    m_callbacks.append(callback);
}

void DeterministicScheduler::clearCallbacks()
{
    m_callbacks.clear();
}

void DeterministicScheduler::onTimerTick()
{
    const qint64 currentTimeUs = m_tickTimer.nsecsElapsed() / 1000;
    const qint64 elapsedMs = m_elapsedTimer.elapsed();

    // Calculate jitter
    if (m_lastTickTimeUs > 0) {
        const qint64 expectedIntervalUs = m_tickIntervalMs * 1000;
        const qint64 actualIntervalUs = currentTimeUs - m_lastTickTimeUs;
        const double jitterUs = std::abs(
            static_cast<double>(actualIntervalUs - expectedIntervalUs));

        // Update average jitter (exponential moving average)
        const double alpha = 0.1;
        m_stats.avgJitterUs = m_stats.avgJitterUs * (1.0 - alpha) + jitterUs * alpha;

        // Check for missed ticks
        if (actualIntervalUs > expectedIntervalUs * 2) {
            int missedCount = static_cast<int>(actualIntervalUs / expectedIntervalUs) - 1;
            m_stats.missedTicks += static_cast<uint64_t>(missedCount);
            emit tickMissed(missedCount);
        }

        // Check jitter threshold
        if (jitterUs > m_jitterThresholdUs) {
            emit jitterExceeded(jitterUs);
        }
    }
    m_lastTickTimeUs = currentTimeUs;

    // Measure tick execution time
    QElapsedTimer execTimer;
    execTimer.start();

    m_stats.tickCount++;

    // Execute callbacks
    for (const TickCallback& callback : qAsConst(m_callbacks)) {
        callback(m_stats.tickCount, elapsedMs);
    }

    // Emit tick signal
    emit tick(m_stats.tickCount, elapsedMs);

    // Update execution time statistics
    const double execTimeUs = static_cast<double>(execTimer.nsecsElapsed()) / 1000.0;
    const double alpha = 0.1;
    m_stats.avgTickDurationUs = m_stats.avgTickDurationUs * (1.0 - alpha) +
                                 execTimeUs * alpha;
    if (execTimeUs > m_stats.maxTickDurationUs) {
        m_stats.maxTickDurationUs = execTimeUs;
    }
}

} // namespace sched
} // namespace automotive
