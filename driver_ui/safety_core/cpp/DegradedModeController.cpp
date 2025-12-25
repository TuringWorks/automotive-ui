// DegradedModeController.cpp
// Degraded mode controller implementation

#include "DegradedModeController.h"

namespace automotive {
namespace driver {

DegradedModeController::DegradedModeController(signal::SignalHub* signalHub, QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
{
    Q_ASSERT(signalHub != nullptr);

    connect(m_signalHub, &signal::SignalHub::degradedModeChanged,
            this, &DegradedModeController::onDegradedModeChanged);
}

DegradedModeController::~DegradedModeController() = default;

void DegradedModeController::processTick(qint64 currentTimeMs)
{
    Q_UNUSED(currentTimeMs)

    if (!m_levelForced) {
        updateLevel();
    }
}

void DegradedModeController::forceLevel(int level)
{
    m_levelForced = true;
    m_forcedLevel = static_cast<DegradedLevel>(level);
    setLevel(m_forcedLevel);
}

void DegradedModeController::clearForcedLevel()
{
    m_levelForced = false;
    updateLevel();
}

void DegradedModeController::onDegradedModeChanged(bool degraded)
{
    Q_UNUSED(degraded)
    if (!m_levelForced) {
        updateLevel();
    }
}

void DegradedModeController::updateLevel()
{
    int invalidCount = m_signalHub->invalidSignalCount();
    bool degraded = m_signalHub->isDegradedMode();

    DegradedLevel newLevel;

    if (invalidCount == 0) {
        newLevel = DegradedLevel::Normal;
    } else if (invalidCount <= 2 && !degraded) {
        newLevel = DegradedLevel::Partial;
    } else if (invalidCount <= 5) {
        newLevel = DegradedLevel::Degraded;
    } else {
        newLevel = DegradedLevel::Minimal;
    }

    setLevel(newLevel);
}

void DegradedModeController::setLevel(DegradedLevel newLevel)
{
    if (newLevel != m_level) {
        bool wasDegraded = isDegraded();
        DegradedLevel oldLevel = m_level;

        m_level = newLevel;

        // Update message
        m_degradedMessage = levelToMessage(m_level);
        emit degradedMessageChanged(m_degradedMessage);

        // Emit transitions
        if (newLevel >= DegradedLevel::Degraded && oldLevel < DegradedLevel::Degraded) {
            emit enteringDegradedMode(newLevel);
        } else if (newLevel < DegradedLevel::Degraded && oldLevel >= DegradedLevel::Degraded) {
            emit exitingDegradedMode();
        }

        emit levelChanged(m_level);

        if (isDegraded() != wasDegraded) {
            emit isDegradedChanged(isDegraded());
        }

        emit visibilityChanged();
    }
}

QString DegradedModeController::levelToMessage(DegradedLevel level) const
{
    switch (level) {
    case DegradedLevel::Normal:
        return QString();
    case DegradedLevel::Partial:
        return QStringLiteral("Limited display - some data unavailable");
    case DegradedLevel::Degraded:
        return QStringLiteral("Degraded mode - critical signals unavailable");
    case DegradedLevel::Minimal:
        return QStringLiteral("Minimal display - signal failure");
    }
    return QString();
}

} // namespace driver
} // namespace automotive
