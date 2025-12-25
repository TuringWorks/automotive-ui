// SafetyMonitor.cpp
// Safety monitor implementation

#include "SafetyMonitor.h"

namespace automotive {
namespace driver {

SafetyMonitor::SafetyMonitor(signal::SignalHub* signalHub,
                             sched::DeterministicScheduler* scheduler,
                             QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
    , m_scheduler(scheduler)
{
    Q_ASSERT(signalHub != nullptr);

    if (m_scheduler) {
        connect(m_scheduler, &sched::DeterministicScheduler::tickMissed,
                this, &SafetyMonitor::onTickMissed);
        connect(m_scheduler, &sched::DeterministicScheduler::jitterExceeded,
                this, &SafetyMonitor::onJitterExceeded);
    }

    m_frameTimer.start();
}

SafetyMonitor::~SafetyMonitor() = default;

void SafetyMonitor::recordFrame()
{
    m_frameCount++;
}

void SafetyMonitor::processTick(qint64 currentTimeMs)
{
    // Calculate frame rate every second
    if (currentTimeMs - m_lastFrameCheckMs >= 1000) {
        double elapsed = static_cast<double>(currentTimeMs - m_lastFrameCheckMs) / 1000.0;
        double newRate = m_frameCount / elapsed;

        if (qAbs(newRate - m_frameRate) > 1.0) {
            m_frameRate = newRate;
            emit frameRateChanged(m_frameRate);
        }

        // Check for missed frames
        int expectedFrames = static_cast<int>(TARGET_FRAME_RATE * elapsed);
        int missed = expectedFrames - m_frameCount;
        if (missed > 0 && missed != m_missedFrames) {
            m_missedFrames += missed;
            emit missedFramesChanged(m_missedFrames);
        }

        m_frameCount = 0;
        m_lastFrameCheckMs = currentTimeMs;
    }

    updateState();
}

QVariantMap SafetyMonitor::getDiagnostics() const
{
    QVariantMap diag;
    diag[QStringLiteral("state")] = static_cast<int>(m_state);
    diag[QStringLiteral("frameRate")] = m_frameRate;
    diag[QStringLiteral("missedFrames")] = m_missedFrames;
    diag[QStringLiteral("missedTicks")] = m_missedTicks;
    diag[QStringLiteral("maxJitterUs")] = m_maxJitterUs;
    diag[QStringLiteral("invalidSignals")] = m_signalHub->invalidSignalCount();
    diag[QStringLiteral("degradedMode")] = m_signalHub->isDegradedMode();

    if (m_scheduler) {
        auto stats = m_scheduler->statistics();
        diag[QStringLiteral("schedulerTicks")] = static_cast<qint64>(stats.tickCount);
        diag[QStringLiteral("schedulerMissed")] = static_cast<qint64>(stats.missedTicks);
        diag[QStringLiteral("avgTickDurationUs")] = stats.avgTickDurationUs;
    }

    return diag;
}

void SafetyMonitor::onTickMissed(int count)
{
    m_missedTicks += count;
    emit healthWarning(QString::fromLatin1("Scheduler missed %1 ticks").arg(count));
    updateState();
}

void SafetyMonitor::onJitterExceeded(double jitterUs)
{
    if (jitterUs > m_maxJitterUs) {
        m_maxJitterUs = jitterUs;
    }

    if (jitterUs > MAX_JITTER_MS * 1000) {
        emit healthWarning(QString::fromLatin1("High jitter detected: %1 ms")
                               .arg(jitterUs / 1000.0, 0, 'f', 1));
    }
}

void SafetyMonitor::updateState()
{
    MonitorState newState = MonitorState::Ok;

    // Check frame rate
    if (m_frameRate > 0 && m_frameRate < MIN_FRAME_RATE) {
        newState = MonitorState::Warning;
    }

    // Check missed frames
    if (m_missedFrames > MAX_MISSED_FRAMES) {
        newState = MonitorState::Warning;
    }

    // Check signal health
    if (m_signalHub->isDegradedMode()) {
        newState = MonitorState::Warning;
    }

    // Multiple issues = Fault
    int issues = 0;
    if (m_frameRate > 0 && m_frameRate < MIN_FRAME_RATE) issues++;
    if (m_missedFrames > MAX_MISSED_FRAMES * 2) issues++;
    if (m_signalHub->invalidSignalCount() > 5) issues++;
    if (m_missedTicks > 10) issues++;

    if (issues >= 2) {
        newState = MonitorState::Fault;
    }

    if (newState != m_state) {
        bool wasHealthy = isHealthy();
        m_state = newState;
        emit stateChanged(m_state);

        if (isHealthy() != wasHealthy) {
            emit isHealthyChanged(isHealthy());
        }

        if (m_state == MonitorState::Fault) {
            emit healthFault(QStringLiteral("Multiple health issues detected"));
        }
    }
}

} // namespace driver
} // namespace automotive
