// HmiEventLog.h
// Auditable ADAS Event Timeline
// Part of: Shared ADAS Platform Layer
// Security: Tamper-evident event logging for ADAS events
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.1

#ifndef AUTOMOTIVE_HMI_EVENT_LOG_H
#define AUTOMOTIVE_HMI_EVENT_LOG_H

#include "AdasTypes.h"
#include <QObject>
#include <QString>
#include <QVector>
#include <QMutex>
#include <QDateTime>
#include <QJsonObject>
#include <QElapsedTimer>

namespace automotive {
namespace adas {

/**
 * @brief Event category for classification
 */
enum class EventCategory : uint8_t {
    StateChange = 0,        ///< ADAS state transitions
    Engagement,             ///< Engage/disengage events
    Takeover,               ///< Takeover requests and responses
    Perception,             ///< Perception quality changes
    Odd,                    ///< ODD boundary events
    DriverMonitoring,       ///< DMS events
    Fault,                  ///< Faults and errors
    Security,               ///< Security-related events
    UserAction,             ///< User interactions
    System                  ///< System events
};

/**
 * @brief Event severity level
 */
enum class EventSeverity : uint8_t {
    Debug = 0,
    Info,
    Warning,
    Error,
    Critical
};

/**
 * @brief Single HMI event record
 */
struct HmiEvent {
    // Identity
    uint64_t sequenceId{0};         ///< Monotonic event ID
    qint64 timestampMs{0};          ///< Monotonic timestamp
    QDateTime wallClock;            ///< Wall clock time for display

    // Classification
    EventCategory category{EventCategory::System};
    EventSeverity severity{EventSeverity::Info};
    QString eventType;              ///< Specific event type string

    // Content
    QString summary;                ///< Human-readable summary
    QString details;                ///< Detailed information
    QJsonObject data;               ///< Structured event data

    // State context
    AdasHmiState hmiStateAtEvent{AdasHmiState::Off};
    AutomationLevel levelAtEvent{AutomationLevel::L0_NoAutomation};

    // Integrity
    QString checksum;               ///< Hash for tamper detection

    /**
     * @brief Convert to JSON for serialization
     */
    QJsonObject toJson() const;

    /**
     * @brief Create from JSON
     */
    static HmiEvent fromJson(const QJsonObject& json);
};

/**
 * @brief HMI Event Log - Auditable Timeline
 *
 * Provides tamper-evident logging of ADAS events for:
 * - Post-drive analysis
 * - Incident investigation
 * - Regulatory compliance
 *
 * Security Requirements:
 * - CR-INF-ADAS-010: Logs must be tamper-evident and access-controlled
 * - CR-INF-ADAS-111: Privileged actions generate audit events
 *
 * Safety Requirements:
 * - Provides disengagement cause tracking (SOTIF-UI-003)
 */
class HmiEventLog : public QObject {
    Q_OBJECT

    Q_PROPERTY(int eventCount READ eventCount NOTIFY eventsChanged)
    Q_PROPERTY(bool hasWarnings READ hasWarnings NOTIFY eventsChanged)
    Q_PROPERTY(bool hasErrors READ hasErrors NOTIFY eventsChanged)

public:
    explicit HmiEventLog(QObject* parent = nullptr);
    ~HmiEventLog() override;

    // Event logging
    void logStateChange(AdasHmiState oldState, AdasHmiState newState,
                        const QString& reason = QString());
    void logEngagement(bool engaged, AutomationLevel level,
                       const QString& reason = QString());
    void logDisengagement(const QString& cause, const QJsonObject& context = QJsonObject());
    void logTakeoverRequest(TakeoverUrgency urgency, const QString& reason);
    void logTakeoverResponse(bool acknowledged, double responseTimeSec);
    void logMrmEvent(const QString& reason, const QString& outcome);
    void logPerceptionChange(ConfidenceLevel oldLevel, ConfidenceLevel newLevel,
                             const QString& sensor);
    void logOddEvent(OddStatus status, const QString& reason);
    void logDriverMonitoring(AttentionState state, const QString& details);
    void logFault(const QString& faultCode, const QString& description,
                  EventSeverity severity = EventSeverity::Error);
    void logSecurityEvent(const QString& eventType, const QString& details);
    void logUserAction(const QString& action, const QJsonObject& context = QJsonObject());

    // Generic logging
    void logEvent(EventCategory category, EventSeverity severity,
                  const QString& eventType, const QString& summary,
                  const QString& details = QString(),
                  const QJsonObject& data = QJsonObject());

    // Query
    int eventCount() const;
    bool hasWarnings() const;
    bool hasErrors() const;
    QVector<HmiEvent> recentEvents(int count = 50) const;
    QVector<HmiEvent> eventsByCategory(EventCategory category, int limit = 100) const;
    QVector<HmiEvent> eventsInRange(qint64 startMs, qint64 endMs) const;

    // Session management
    void startNewSession(const QString& sessionId = QString());
    QString currentSessionId() const;
    qint64 sessionStartMs() const;

    // Export
    QJsonObject exportToJson() const;
    bool exportToFile(const QString& filePath) const;

    // Configuration
    void setMaxEvents(int max);
    void setCurrentState(AdasHmiState state, AutomationLevel level);

    // Integrity verification
    bool verifyIntegrity() const;
    QString computeSessionChecksum() const;

    static constexpr int DEFAULT_MAX_EVENTS = 10000;

signals:
    void eventsChanged();
    void eventLogged(const HmiEvent& event);
    void warningLogged(const HmiEvent& event);
    void errorLogged(const HmiEvent& event);

private:
    QString computeEventChecksum(const HmiEvent& event) const;
    qint64 currentMonotonicTimeMs() const;

    mutable QMutex m_mutex;

    QVector<HmiEvent> m_events;
    int m_maxEvents{DEFAULT_MAX_EVENTS};
    uint64_t m_nextSequenceId{1};

    QString m_sessionId;
    qint64 m_sessionStartMs{0};
    QString m_previousChecksum;

    // Current state context
    AdasHmiState m_currentHmiState{AdasHmiState::Off};
    AutomationLevel m_currentLevel{AutomationLevel::L0_NoAutomation};

    // Counters
    int m_warningCount{0};
    int m_errorCount{0};

    QElapsedTimer m_monotonicTimer;
};

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::EventCategory)
Q_DECLARE_METATYPE(automotive::adas::EventSeverity)
Q_DECLARE_METATYPE(automotive::adas::HmiEvent)

#endif // AUTOMOTIVE_HMI_EVENT_LOG_H
