// TakeoverManager.h
// L3+ Takeover Request State Machine
// Part of: Shared ADAS Platform Layer
// Safety: Critical takeover pipeline management
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.3, 4.6

#ifndef AUTOMOTIVE_TAKEOVER_MANAGER_H
#define AUTOMOTIVE_TAKEOVER_MANAGER_H

#include "AdasTypes.h"
#include "AdasEngagement.h"
#include <QObject>
#include <QElapsedTimer>
#include <QMutex>

namespace automotive {
namespace adas {

/**
 * @brief Takeover state machine states
 */
enum class TakeoverState : uint8_t {
    Inactive = 0,           ///< No takeover in progress
    PreWarning,             ///< ODD ending soon - advisory
    RequestActive,          ///< Takeover requested - countdown running
    Escalated,              ///< Driver not responding - urgent
    CriticalEscalation,     ///< Final warning before MRM
    MrmInitiated,           ///< MRM started due to no response
    Completed               ///< Driver has taken over successfully
};

/**
 * @brief Takeover Manager - L3+ Takeover Pipeline
 *
 * Manages the complete takeover request flow for L3+ automation.
 *
 * Safety Requirements:
 * - SR-CL-ADAS-120: Takeover banner non-occludable with escalation
 * - SR-CL-ADAS-121: Urgency levels map to consistent HMI patterns
 * - SR-CL-ADAS-122: "Immediate Action Required" at threshold
 * - SR-CL-ADAS-130: MRM shows persistent state, suppresses overlays
 *
 * Flow:
 * PreWarning → RequestActive → Escalated → CriticalEscalation → MrmInitiated
 *                    ↓
 *                Completed (driver took over)
 */
class TakeoverManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool active READ isActive NOTIFY stateChanged)
    Q_PROPERTY(int state READ stateInt NOTIFY stateChanged)
    Q_PROPERTY(int urgency READ urgencyInt NOTIFY urgencyChanged)
    Q_PROPERTY(double countdown READ countdown NOTIFY countdownChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QString reason READ reason NOTIFY reasonChanged)
    Q_PROPERTY(bool requiresImmediateAction READ requiresImmediateAction NOTIFY urgencyChanged)
    Q_PROPERTY(bool mrmActive READ isMrmActive NOTIFY stateChanged)
    Q_PROPERTY(int escalationLevel READ escalationLevel NOTIFY escalationChanged)

public:
    explicit TakeoverManager(QObject* parent = nullptr);
    ~TakeoverManager() override;

    // State access
    TakeoverState state() const;
    int stateInt() const { return static_cast<int>(state()); }
    TakeoverUrgency urgency() const;
    int urgencyInt() const { return static_cast<int>(urgency()); }
    bool isActive() const;
    bool isMrmActive() const;
    bool requiresImmediateAction() const;

    // Countdown and messaging
    double countdown() const;
    QString message() const;
    QString reason() const;
    int escalationLevel() const;

    // Audio/haptic hooks
    bool shouldPlayAudio() const;
    bool shouldActivateHaptic() const;

    /**
     * @brief Update from incoming takeover request message
     */
    bool updateFromRequest(const TakeoverRequest& request);

    /**
     * @brief Process tick (update countdown, check escalation)
     */
    void processTick(uint64_t tickNumber, qint64 elapsedMs);

    /**
     * @brief Acknowledge takeover (driver responded)
     */
    void acknowledgeTakeover();

    /**
     * @brief Cancel takeover request (ODD re-entered, etc.)
     */
    void cancelTakeover();

    /**
     * @brief Force MRM state (for testing/fault injection)
     */
    void forceMrm(const QString& reason);

    /**
     * @brief Reset to inactive state
     */
    void reset();

    // Configuration
    static constexpr double PRE_WARNING_THRESHOLD_SEC = 30.0;
    static constexpr double REQUEST_TIMEOUT_SEC = 10.0;
    static constexpr double ESCALATION_TIMEOUT_SEC = 5.0;
    static constexpr double CRITICAL_THRESHOLD_SEC = 3.0;
    static constexpr int MAX_ESCALATION_LEVEL = 3;

signals:
    void stateChanged(TakeoverState newState);
    void urgencyChanged(TakeoverUrgency urgency);
    void countdownChanged(double remaining);
    void messageChanged(const QString& message);
    void reasonChanged(const QString& reason);
    void escalationChanged(int level);

    // Action signals
    void takeoverRequested();
    void takeoverEscalated(int level);
    void mrmStarted(const QString& reason);
    void takeoverCompleted();
    void takeoverCancelled();

    // Audio/haptic triggers
    void playTakeoverAudio(TakeoverUrgency urgency);
    void activateHaptic(TakeoverUrgency urgency);

private:
    void transitionTo(TakeoverState newState);
    void updateUrgencyFromState();
    void checkEscalation(qint64 elapsedMs);
    void updateMessage();
    qint64 currentMonotonicTimeMs() const;

    mutable QMutex m_mutex;

    TakeoverState m_state{TakeoverState::Inactive};
    TakeoverUrgency m_urgency{TakeoverUrgency::None};

    // Countdown tracking
    double m_initialCountdown{0.0};
    double m_remainingCountdown{0.0};
    qint64 m_countdownStartMs{0};

    // Escalation
    int m_escalationLevel{0};
    qint64 m_lastEscalationMs{0};

    // Messaging
    QString m_message;
    QString m_reason;
    QString m_requiredAction;

    // Audio/haptic state
    bool m_audioActive{false};
    bool m_hapticActive{false};

    QElapsedTimer m_monotonicTimer;
};

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::TakeoverState)

#endif // AUTOMOTIVE_TAKEOVER_MANAGER_H
