// AdasStateService.cpp
// Authoritative ADAS State Machine Implementation
// Safety: Deterministic state management with timing guarantees

#include "AdasStateService.h"
#include <QMutexLocker>

namespace automotive {
namespace adas {

// Forward declaration for helper function
static AdasHmiState deriveHmiState(const AdasEngagement& engagement);

AdasStateService::AdasStateService(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
    m_lastStateChangeMs = currentMonotonicTimeMs();
}

AdasStateService::~AdasStateService() = default;

qint64 AdasStateService::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

AdasHmiState AdasStateService::hmiState() const
{
    QMutexLocker locker(&m_mutex);
    return m_hmiState;
}

AutomationLevel AdasStateService::automationLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.level;
}

bool AdasStateService::isAvailable() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.available && m_engagement.metadata.isValid();
}

bool AdasStateService::isEngaged() const
{
    QMutexLocker locker(&m_mutex);
    // SR-CL-ADAS-100: Only show engaged if truly engaged AND available AND valid
    return m_engagement.canShowEngaged();
}

bool AdasStateService::isDegraded() const
{
    QMutexLocker locker(&m_mutex);
    return m_hmiState == AdasHmiState::Degraded || m_forcedDegraded;
}

QString AdasStateService::stateText() const
{
    QMutexLocker locker(&m_mutex);
    return adasHmiStateToString(m_hmiState);
}

QString AdasStateService::responsibilityText() const
{
    QMutexLocker locker(&m_mutex);
    return responsibilityOwner(m_engagement.level, m_engagement.engaged);
}

double AdasStateService::setSpeed() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.setSpeedKph;
}

double AdasStateService::followingGap() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.followingGapSeconds;
}

QString AdasStateService::featureSummary() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.featureSummary();
}

bool AdasStateService::isAccActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.accActive && m_engagement.canShowEngaged();
}

bool AdasStateService::isLkaActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.lkaActive && m_engagement.canShowEngaged();
}

bool AdasStateService::isLcaActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.lcaActive && m_engagement.canShowEngaged();
}

bool AdasStateService::isBsmActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement.bsmActive;
}

AdasEngagement AdasStateService::engagement() const
{
    QMutexLocker locker(&m_mutex);
    return m_engagement;
}

OddData AdasStateService::oddStatus() const
{
    QMutexLocker locker(&m_mutex);
    return m_oddStatus;
}

DmsStatus AdasStateService::dmsStatus() const
{
    QMutexLocker locker(&m_mutex);
    return m_dmsStatus;
}

bool AdasStateService::updateEngagement(const AdasEngagement& engagement)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    // CR-ADAS-102: Anti-replay check
    if (!validateSequence(engagement.metadata.sequenceNumber)) {
        emit integrityFailure(QStringLiteral("Sequence number regression detected"));
        return false;
    }

    // CR-ADAS-101: Freshness check
    if (!validateFreshness(engagement.metadata.timestampMs)) {
        emit integrityFailure(QStringLiteral("Message outside freshness window"));
        return false;
    }

    AdasHmiState oldState;
    AdasHmiState newState;

    {
        QMutexLocker locker(&m_mutex);
        oldState = m_hmiState;
        m_engagement = engagement;
        m_lastEngagementUpdateMs = currentTime;
        m_lastEngagementSeq = engagement.metadata.sequenceNumber;
        m_staleReported = false;

        // Derive HMI state from engagement
        newState = deriveHmiState(engagement);
    }

    emit engagementUpdated(engagement);

    // Handle state transition
    if (newState != oldState) {
        if (isTransitionAllowed(oldState, newState)) {
            performStateTransition(newState);
        } else {
            emit stateTransitionBlocked(oldState, newState,
                QStringLiteral("Transition not allowed"));
        }
    }

    updateDerivedState();
    return true;
}

bool AdasStateService::updateOddStatus(const OddData& odd)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    if (odd.metadata.sequenceNumber <= m_lastOddSeq &&
        m_lastOddSeq != 0) {
        return false; // Anti-replay
    }

    {
        QMutexLocker locker(&m_mutex);
        m_oddStatus = odd;
        m_lastOddUpdateMs = currentTime;
        m_lastOddSeq = odd.metadata.sequenceNumber;
    }

    emit oddStatusUpdated(odd);
    return true;
}

bool AdasStateService::updateDmsStatus(const DmsStatus& dms)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    if (dms.metadata.sequenceNumber <= m_lastDmsSeq &&
        m_lastDmsSeq != 0) {
        return false; // Anti-replay
    }

    {
        QMutexLocker locker(&m_mutex);
        m_dmsStatus = dms;
        m_lastDmsUpdateMs = currentTime;
        m_lastDmsSeq = dms.metadata.sequenceNumber;
    }

    emit dmsStatusUpdated(dms);
    return true;
}

void AdasStateService::processTick(uint64_t /*tickNumber*/, qint64 elapsedMs)
{
    checkFreshness(elapsedMs);
}

bool AdasStateService::requestStateTransition(AdasHmiState targetState)
{
    QMutexLocker locker(&m_mutex);

    if (!isTransitionAllowed(m_hmiState, targetState)) {
        emit stateTransitionBlocked(m_hmiState, targetState,
            QStringLiteral("Invalid state transition"));
        return false;
    }

    locker.unlock();
    performStateTransition(targetState);
    return true;
}

void AdasStateService::forceDegradedMode(bool degraded)
{
    {
        QMutexLocker locker(&m_mutex);
        if (m_forcedDegraded == degraded) {
            return;
        }
        m_forcedDegraded = degraded;
    }
    emit degradedChanged(degraded);

    if (degraded) {
        performStateTransition(AdasHmiState::Degraded);
    }
}

qint64 AdasStateService::lastStateChangeMs() const
{
    QMutexLocker locker(&m_mutex);
    return m_lastStateChangeMs;
}

qint64 AdasStateService::msSinceLastUpdate() const
{
    QMutexLocker locker(&m_mutex);
    return currentMonotonicTimeMs() - m_lastEngagementUpdateMs;
}

bool AdasStateService::isTransitionAllowed(AdasHmiState from, AdasHmiState to) const
{
    // State machine transition rules per spec Section 4.2
    switch (from) {
        case AdasHmiState::Off:
            return to == AdasHmiState::Available ||
                   to == AdasHmiState::Degraded ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::Available:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::EngagedL1 ||
                   to == AdasHmiState::EngagedL2 ||
                   to == AdasHmiState::AvailableL3 ||
                   to == AdasHmiState::Degraded ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::EngagedL1:
        case AdasHmiState::EngagedL2:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::Available ||
                   to == AdasHmiState::EngagedL1 ||
                   to == AdasHmiState::EngagedL2 ||
                   to == AdasHmiState::Degraded ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::AvailableL3:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::Available ||
                   to == AdasHmiState::EngagedL3 ||
                   to == AdasHmiState::Degraded ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::EngagedL3:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::AvailableL3 ||
                   to == AdasHmiState::TakeoverRequested ||
                   to == AdasHmiState::Degraded ||
                   to == AdasHmiState::MrmActive ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::TakeoverRequested:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::Available ||
                   to == AdasHmiState::EngagedL3 ||
                   to == AdasHmiState::MrmActive ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::MrmActive:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::Available ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::Degraded:
            return to == AdasHmiState::Off ||
                   to == AdasHmiState::Available ||
                   to == AdasHmiState::FaultLockout;

        case AdasHmiState::FaultLockout:
            // Can only exit via explicit reset (off)
            return to == AdasHmiState::Off;
    }

    return false;
}

void AdasStateService::performStateTransition(AdasHmiState newState)
{
    AdasHmiState oldState;
    {
        QMutexLocker locker(&m_mutex);
        oldState = m_hmiState;
        if (oldState == newState) {
            return;
        }

        m_hmiState = newState;
        m_lastStateChangeMs = currentMonotonicTimeMs();
    }

    // Emit state change signals
    emit hmiStateChanged(newState);
    emit stateTextChanged(adasHmiStateToString(newState));

    // Update derived properties
    if ((oldState == AdasHmiState::EngagedL1 ||
         oldState == AdasHmiState::EngagedL2 ||
         oldState == AdasHmiState::EngagedL3) !=
        (newState == AdasHmiState::EngagedL1 ||
         newState == AdasHmiState::EngagedL2 ||
         newState == AdasHmiState::EngagedL3)) {
        emit engagedChanged(isEngaged());
    }

    if ((oldState == AdasHmiState::Available ||
         oldState == AdasHmiState::AvailableL3) !=
        (newState == AdasHmiState::Available ||
         newState == AdasHmiState::AvailableL3)) {
        emit availableChanged(isAvailable());
    }

    if ((oldState == AdasHmiState::Degraded) !=
        (newState == AdasHmiState::Degraded)) {
        emit degradedChanged(newState == AdasHmiState::Degraded);
    }
}

void AdasStateService::updateDerivedState()
{
    emit setSpeedChanged(setSpeed());
    emit followingGapChanged(followingGap());
    emit featuresChanged();
    emit responsibilityChanged(responsibilityText());
}

void AdasStateService::checkFreshness(qint64 currentTimeMs)
{
    QMutexLocker locker(&m_mutex);

    // SR-CL-ADAS-111: Staleness indication within 300ms
    const qint64 engagementAge = currentTimeMs - m_lastEngagementUpdateMs;

    if (engagementAge > FRESHNESS_WINDOW_MS && !m_staleReported) {
        m_staleReported = true;

        // CR-ADAS-103: Default to OFF on stale data
        if (m_hmiState != AdasHmiState::Off &&
            m_hmiState != AdasHmiState::FaultLockout) {

            m_engagement.metadata.valid = false;
            locker.unlock();

            emit staleStateDetected();
            performStateTransition(AdasHmiState::Degraded);
        }
    }
}

bool AdasStateService::validateSequence(uint32_t newSeq)
{
    // Allow first message or monotonic increase
    // Also allow wrap-around (new seq near 0 when old is near max)
    if (m_lastEngagementSeq == 0) {
        return true;
    }

    // Normal monotonic check
    if (newSeq > m_lastEngagementSeq) {
        return true;
    }

    // Wrap-around detection
    constexpr uint32_t wrapThreshold = 0x80000000;
    if (m_lastEngagementSeq > wrapThreshold && newSeq < 1000) {
        return true;
    }

    return false;
}

bool AdasStateService::validateFreshness(qint64 sourceTimestamp)
{
    // Freshness check against source timestamp
    // Note: This requires synchronized clocks; for now accept all
    Q_UNUSED(sourceTimestamp)
    return true;
}

// Helper function to derive HMI state from engagement data
static AdasHmiState deriveHmiState(const AdasEngagement& engagement)
{
    if (!engagement.metadata.isValid()) {
        return AdasHmiState::Off;
    }

    if (!engagement.available) {
        return AdasHmiState::Off;
    }

    if (engagement.engaged) {
        switch (engagement.level) {
            case AutomationLevel::L1_DriverAssistance:
                return AdasHmiState::EngagedL1;
            case AutomationLevel::L2_PartialAutomation:
                return AdasHmiState::EngagedL2;
            case AutomationLevel::L3_ConditionalAutomation:
                return AdasHmiState::EngagedL3;
            case AutomationLevel::L4_HighAutomation:
            case AutomationLevel::L5_FullAutomation:
                return AdasHmiState::EngagedL3; // Use L3 view for L4/L5
            default:
                return AdasHmiState::Off;
        }
    }

    // Available but not engaged
    if (engagement.level >= AutomationLevel::L3_ConditionalAutomation) {
        return AdasHmiState::AvailableL3;
    }

    return AdasHmiState::Available;
}

} // namespace adas
} // namespace automotive
