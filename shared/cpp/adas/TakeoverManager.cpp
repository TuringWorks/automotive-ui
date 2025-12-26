// TakeoverManager.cpp
// L3+ Takeover Request State Machine Implementation
// Safety: Critical takeover pipeline with escalation

#include "TakeoverManager.h"
#include <QMutexLocker>

namespace automotive {
namespace adas {

TakeoverManager::TakeoverManager(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
}

TakeoverManager::~TakeoverManager() = default;

qint64 TakeoverManager::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

TakeoverState TakeoverManager::state() const
{
    QMutexLocker locker(&m_mutex);
    return m_state;
}

TakeoverUrgency TakeoverManager::urgency() const
{
    QMutexLocker locker(&m_mutex);
    return m_urgency;
}

bool TakeoverManager::isActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_state != TakeoverState::Inactive &&
           m_state != TakeoverState::Completed;
}

bool TakeoverManager::isMrmActive() const
{
    QMutexLocker locker(&m_mutex);
    return m_state == TakeoverState::MrmInitiated;
}

bool TakeoverManager::requiresImmediateAction() const
{
    QMutexLocker locker(&m_mutex);
    return m_urgency >= TakeoverUrgency::Immediate ||
           m_remainingCountdown <= CRITICAL_THRESHOLD_SEC;
}

double TakeoverManager::countdown() const
{
    QMutexLocker locker(&m_mutex);
    return m_remainingCountdown;
}

QString TakeoverManager::message() const
{
    QMutexLocker locker(&m_mutex);
    return m_message;
}

QString TakeoverManager::reason() const
{
    QMutexLocker locker(&m_mutex);
    return m_reason;
}

int TakeoverManager::escalationLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_escalationLevel;
}

bool TakeoverManager::shouldPlayAudio() const
{
    QMutexLocker locker(&m_mutex);
    return m_audioActive;
}

bool TakeoverManager::shouldActivateHaptic() const
{
    QMutexLocker locker(&m_mutex);
    return m_hapticActive;
}

bool TakeoverManager::updateFromRequest(const TakeoverRequest& request)
{
    if (!request.active) {
        if (isActive()) {
            cancelTakeover();
        }
        return true;
    }

    const qint64 currentTime = currentMonotonicTimeMs();

    {
        QMutexLocker locker(&m_mutex);

        m_reason = request.reason;
        m_requiredAction = request.requiredAction;
        m_initialCountdown = request.countdownSec;
        m_remainingCountdown = request.countdownSec;
        m_countdownStartMs = currentTime;
        m_audioActive = request.audioActive;
        m_hapticActive = request.hapticActive;
    }

    // Determine initial state based on urgency
    TakeoverState targetState;
    switch (request.urgency) {
        case TakeoverUrgency::Advisory:
            targetState = TakeoverState::PreWarning;
            break;
        case TakeoverUrgency::Warning:
            targetState = TakeoverState::RequestActive;
            break;
        case TakeoverUrgency::Immediate:
            targetState = TakeoverState::Escalated;
            break;
        case TakeoverUrgency::Critical:
            targetState = TakeoverState::CriticalEscalation;
            break;
        default:
            targetState = TakeoverState::Inactive;
    }

    if (targetState != TakeoverState::Inactive) {
        transitionTo(targetState);
    }

    updateMessage();
    return true;
}

void TakeoverManager::processTick(uint64_t /*tickNumber*/, qint64 elapsedMs)
{
    QMutexLocker locker(&m_mutex);

    if (m_state == TakeoverState::Inactive ||
        m_state == TakeoverState::Completed ||
        m_state == TakeoverState::MrmInitiated) {
        return;
    }

    // Update countdown
    if (m_countdownStartMs > 0 && m_initialCountdown > 0) {
        const double elapsedSec = (elapsedMs - m_countdownStartMs) / 1000.0;
        m_remainingCountdown = std::max(0.0, m_initialCountdown - elapsedSec);

        locker.unlock();
        emit countdownChanged(m_remainingCountdown);
        locker.relock();
    }

    locker.unlock();
    checkEscalation(elapsedMs);
}

void TakeoverManager::acknowledgeTakeover()
{
    transitionTo(TakeoverState::Completed);
    emit takeoverCompleted();
}

void TakeoverManager::cancelTakeover()
{
    transitionTo(TakeoverState::Inactive);
    emit takeoverCancelled();
}

void TakeoverManager::forceMrm(const QString& reason)
{
    {
        QMutexLocker locker(&m_mutex);
        m_reason = reason;
    }
    transitionTo(TakeoverState::MrmInitiated);
    emit mrmStarted(reason);
}

void TakeoverManager::reset()
{
    {
        QMutexLocker locker(&m_mutex);
        m_state = TakeoverState::Inactive;
        m_urgency = TakeoverUrgency::None;
        m_remainingCountdown = 0.0;
        m_initialCountdown = 0.0;
        m_escalationLevel = 0;
        m_message.clear();
        m_reason.clear();
        m_requiredAction.clear();
        m_audioActive = false;
        m_hapticActive = false;
    }

    emit stateChanged(TakeoverState::Inactive);
    emit urgencyChanged(TakeoverUrgency::None);
    emit countdownChanged(0.0);
    emit messageChanged(QString());
}

void TakeoverManager::transitionTo(TakeoverState newState)
{
    TakeoverState oldState;
    {
        QMutexLocker locker(&m_mutex);
        oldState = m_state;
        if (oldState == newState) {
            return;
        }
        m_state = newState;

        if (newState == TakeoverState::Inactive ||
            newState == TakeoverState::Completed) {
            m_escalationLevel = 0;
            m_audioActive = false;
            m_hapticActive = false;
        }
    }

    emit stateChanged(newState);
    updateUrgencyFromState();
    updateMessage();

    // Trigger audio/haptic on escalation
    if (newState == TakeoverState::RequestActive) {
        emit takeoverRequested();
        emit playTakeoverAudio(TakeoverUrgency::Warning);
    } else if (newState == TakeoverState::Escalated ||
               newState == TakeoverState::CriticalEscalation) {
        QMutexLocker locker(&m_mutex);
        emit takeoverEscalated(m_escalationLevel);
        emit playTakeoverAudio(m_urgency);
        emit activateHaptic(m_urgency);
    } else if (newState == TakeoverState::MrmInitiated) {
        emit mrmStarted(reason());
    }
}

void TakeoverManager::updateUrgencyFromState()
{
    TakeoverUrgency newUrgency;

    {
        QMutexLocker locker(&m_mutex);
        switch (m_state) {
            case TakeoverState::Inactive:
            case TakeoverState::Completed:
                newUrgency = TakeoverUrgency::None;
                break;
            case TakeoverState::PreWarning:
                newUrgency = TakeoverUrgency::Advisory;
                break;
            case TakeoverState::RequestActive:
                newUrgency = TakeoverUrgency::Warning;
                break;
            case TakeoverState::Escalated:
                newUrgency = TakeoverUrgency::Immediate;
                break;
            case TakeoverState::CriticalEscalation:
            case TakeoverState::MrmInitiated:
                newUrgency = TakeoverUrgency::Critical;
                break;
            default:
                newUrgency = TakeoverUrgency::None;
        }

        if (newUrgency == m_urgency) {
            return;
        }
        m_urgency = newUrgency;
    }

    emit urgencyChanged(newUrgency);
}

void TakeoverManager::checkEscalation(qint64 elapsedMs)
{
    QMutexLocker locker(&m_mutex);

    if (m_state == TakeoverState::Inactive ||
        m_state == TakeoverState::Completed ||
        m_state == TakeoverState::MrmInitiated) {
        return;
    }

    // Check countdown-based escalation
    if (m_remainingCountdown <= 0 && m_initialCountdown > 0) {
        // Countdown expired - initiate MRM
        locker.unlock();
        forceMrm(QStringLiteral("Takeover timeout - no driver response"));
        return;
    }

    // Time-based escalation
    const qint64 timeSinceEscalation = elapsedMs - m_lastEscalationMs;
    const double escalationThresholdMs = ESCALATION_TIMEOUT_SEC * 1000;

    if (timeSinceEscalation > escalationThresholdMs &&
        m_escalationLevel < MAX_ESCALATION_LEVEL) {

        m_escalationLevel++;
        m_lastEscalationMs = elapsedMs;
        m_audioActive = true;
        m_hapticActive = true;

        TakeoverState nextState = m_state;
        switch (m_state) {
            case TakeoverState::PreWarning:
                nextState = TakeoverState::RequestActive;
                break;
            case TakeoverState::RequestActive:
                nextState = TakeoverState::Escalated;
                break;
            case TakeoverState::Escalated:
                nextState = TakeoverState::CriticalEscalation;
                break;
            case TakeoverState::CriticalEscalation:
                locker.unlock();
                forceMrm(QStringLiteral("Maximum escalation reached"));
                return;
            default:
                break;
        }

        if (nextState != m_state) {
            locker.unlock();
            transitionTo(nextState);
            return;
        }
    }

    // Threshold-based urgency bump - SR-CL-ADAS-122
    if (m_remainingCountdown <= CRITICAL_THRESHOLD_SEC &&
        m_state != TakeoverState::CriticalEscalation &&
        m_state != TakeoverState::MrmInitiated) {
        locker.unlock();
        transitionTo(TakeoverState::CriticalEscalation);
    }
}

void TakeoverManager::updateMessage()
{
    QString newMessage;

    {
        QMutexLocker locker(&m_mutex);

        switch (m_state) {
            case TakeoverState::Inactive:
                newMessage.clear();
                break;

            case TakeoverState::PreWarning:
                newMessage = QStringLiteral("Prepare to take over driving");
                if (!m_reason.isEmpty()) {
                    newMessage += QStringLiteral("\n") + m_reason;
                }
                break;

            case TakeoverState::RequestActive:
                newMessage = QStringLiteral("TAKE OVER DRIVING NOW");
                if (m_remainingCountdown > 0) {
                    newMessage += QString::asprintf("\n%.0f seconds", m_remainingCountdown);
                }
                break;

            case TakeoverState::Escalated:
                newMessage = QStringLiteral("TAKE OVER IMMEDIATELY");
                if (!m_requiredAction.isEmpty()) {
                    newMessage += QStringLiteral("\n") + m_requiredAction;
                }
                break;

            case TakeoverState::CriticalEscalation:
                newMessage = QStringLiteral("CRITICAL: TAKE OVER NOW");
                newMessage += QStringLiteral("\nSystem will perform safe stop");
                break;

            case TakeoverState::MrmInitiated:
                newMessage = QStringLiteral("System Performing Safe Stop");
                newMessage += QStringLiteral("\nVehicle is stopping safely");
                break;

            case TakeoverState::Completed:
                newMessage = QStringLiteral("Takeover Complete");
                newMessage += QStringLiteral("\nYou are now driving");
                break;
        }

        if (m_message == newMessage) {
            return;
        }
        m_message = newMessage;
    }

    emit messageChanged(newMessage);
}

} // namespace adas
} // namespace automotive
