// AdasStateService.h
// Authoritative ADAS State Machine
// Part of: Shared ADAS Platform Layer
// Safety: Core safety-critical state management - NO QML logic
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.1, 4.2

#ifndef AUTOMOTIVE_ADAS_STATE_SERVICE_H
#define AUTOMOTIVE_ADAS_STATE_SERVICE_H

#include "AdasTypes.h"
#include "AdasEngagement.h"
#include <QObject>
#include <QElapsedTimer>
#include <QMutex>
#include <functional>

namespace automotive {
namespace adas {

/**
 * @brief Authoritative ADAS State Service
 *
 * This is the single source of truth for ADAS engagement and availability.
 *
 * Safety Requirements:
 * - SR-CL-ADAS-100: Never render ENGAGED unless engaged=true AND available=true AND valid
 * - SR-CL-ADAS-101: Indicate "Unavailable" within 300ms
 * - SR-CL-ADAS-102: Default to OFF/UNAVAILABLE on unknown state
 * - SR-CL-ADAS-110: State changes reflected within 100ms
 * - SR-CL-ADAS-111: Staleness indication within 300ms
 *
 * Cybersecurity Requirements:
 * - CR-ADAS-100: Schema validation on all inputs
 * - CR-ADAS-101: Freshness window enforcement
 * - CR-ADAS-102: Anti-replay via sequence checking
 * - CR-ADAS-103: Default to OFF on integrity failure
 */
class AdasStateService : public QObject {
    Q_OBJECT

    // Core state properties for QML binding
    Q_PROPERTY(int hmiState READ hmiStateInt NOTIFY hmiStateChanged)
    Q_PROPERTY(int automationLevel READ automationLevelInt NOTIFY automationLevelChanged)
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
    Q_PROPERTY(bool engaged READ isEngaged NOTIFY engagedChanged)
    Q_PROPERTY(bool degraded READ isDegraded NOTIFY degradedChanged)
    Q_PROPERTY(QString stateText READ stateText NOTIFY stateTextChanged)
    Q_PROPERTY(QString responsibilityText READ responsibilityText NOTIFY responsibilityChanged)

    // ACC state
    Q_PROPERTY(double setSpeed READ setSpeed NOTIFY setSpeedChanged)
    Q_PROPERTY(double followingGap READ followingGap NOTIFY followingGapChanged)
    Q_PROPERTY(QString featureSummary READ featureSummary NOTIFY featuresChanged)

    // Feature flags
    Q_PROPERTY(bool accActive READ isAccActive NOTIFY featuresChanged)
    Q_PROPERTY(bool lkaActive READ isLkaActive NOTIFY featuresChanged)
    Q_PROPERTY(bool lcaActive READ isLcaActive NOTIFY featuresChanged)
    Q_PROPERTY(bool bsmActive READ isBsmActive NOTIFY featuresChanged)

public:
    explicit AdasStateService(QObject* parent = nullptr);
    ~AdasStateService() override;

    // Prevent copying (safety: single instance)
    AdasStateService(const AdasStateService&) = delete;
    AdasStateService& operator=(const AdasStateService&) = delete;

    // Core state access
    AdasHmiState hmiState() const;
    int hmiStateInt() const { return static_cast<int>(hmiState()); }
    AutomationLevel automationLevel() const;
    int automationLevelInt() const { return static_cast<int>(automationLevel()); }
    bool isAvailable() const;
    bool isEngaged() const;
    bool isDegraded() const;
    QString stateText() const;
    QString responsibilityText() const;

    // ACC state
    double setSpeed() const;
    double followingGap() const;
    QString featureSummary() const;

    // Feature flags
    bool isAccActive() const;
    bool isLkaActive() const;
    bool isLcaActive() const;
    bool isBsmActive() const;

    // Get full engagement data (for detailed views)
    AdasEngagement engagement() const;
    OddData oddStatus() const;
    DmsStatus dmsStatus() const;

    /**
     * @brief Update engagement state from IPC message
     *
     * @param engagement New engagement data
     * @return true if update was accepted
     *
     * Security: Validates freshness, sequence, and schema before accepting.
     */
    bool updateEngagement(const AdasEngagement& engagement);

    /**
     * @brief Update ODD status from IPC message
     */
    bool updateOddStatus(const OddData& odd);

    /**
     * @brief Update DMS status from IPC message
     */
    bool updateDmsStatus(const DmsStatus& dms);

    /**
     * @brief Process periodic tick (check freshness, update state machine)
     *
     * @param tickNumber Current tick number
     * @param elapsedMs Elapsed time since start
     *
     * Must be called at least every 50ms (20Hz) for timing compliance.
     */
    void processTick(uint64_t tickNumber, qint64 elapsedMs);

    /**
     * @brief Request state transition (from vehicle controller)
     *
     * @param targetState Requested state
     * @return true if transition is allowed
     */
    bool requestStateTransition(AdasHmiState targetState);

    /**
     * @brief Force degraded mode (for fault injection testing)
     */
    void forceDegradedMode(bool degraded);

    /**
     * @brief Get last state change timestamp
     */
    qint64 lastStateChangeMs() const;

    /**
     * @brief Get milliseconds since last valid update
     */
    qint64 msSinceLastUpdate() const;

    // Configuration
    static constexpr qint64 FRESHNESS_WINDOW_MS = 300;  ///< SR-CL-ADAS-111
    static constexpr qint64 STATE_CHANGE_DEADLINE_MS = 100; ///< SR-CL-ADAS-110

signals:
    void hmiStateChanged(AdasHmiState newState);
    void automationLevelChanged(AutomationLevel level);
    void availableChanged(bool available);
    void engagedChanged(bool engaged);
    void degradedChanged(bool degraded);
    void stateTextChanged(const QString& text);
    void responsibilityChanged(const QString& text);
    void setSpeedChanged(double speed);
    void followingGapChanged(double gap);
    void featuresChanged();

    // Detailed state signals
    void engagementUpdated(const AdasEngagement& engagement);
    void oddStatusUpdated(const OddData& odd);
    void dmsStatusUpdated(const DmsStatus& dms);

    // Safety signals
    void staleStateDetected();
    void integrityFailure(const QString& reason);
    void stateTransitionBlocked(AdasHmiState from, AdasHmiState to, const QString& reason);

private:
    // State machine transition validation
    bool isTransitionAllowed(AdasHmiState from, AdasHmiState to) const;
    void performStateTransition(AdasHmiState newState);
    void updateDerivedState();
    void checkFreshness(qint64 currentTimeMs);
    qint64 currentMonotonicTimeMs() const;

    // Validation
    bool validateSequence(uint32_t newSeq);
    bool validateFreshness(qint64 sourceTimestamp);

    mutable QMutex m_mutex;

    // Current state
    AdasHmiState m_hmiState{AdasHmiState::Off};
    AdasEngagement m_engagement;
    OddData m_oddStatus;
    DmsStatus m_dmsStatus;

    // State tracking
    qint64 m_lastStateChangeMs{0};
    qint64 m_lastEngagementUpdateMs{0};
    qint64 m_lastOddUpdateMs{0};
    qint64 m_lastDmsUpdateMs{0};

    // Anti-replay
    uint32_t m_lastEngagementSeq{0};
    uint32_t m_lastOddSeq{0};
    uint32_t m_lastDmsSeq{0};

    // Timing
    QElapsedTimer m_monotonicTimer;

    // Flags
    bool m_forcedDegraded{false};
    bool m_staleReported{false};
};

} // namespace adas
} // namespace automotive

#endif // AUTOMOTIVE_ADAS_STATE_SERVICE_H
