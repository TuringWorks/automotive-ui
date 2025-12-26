// HmiEventLog.cpp
// Auditable ADAS Event Timeline Implementation
// Security: Tamper-evident logging with chained checksums

#include "HmiEventLog.h"
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QCryptographicHash>
#include <QUuid>

namespace automotive {
namespace adas {

QJsonObject HmiEvent::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("sequenceId")] = static_cast<qint64>(sequenceId);
    obj[QStringLiteral("timestampMs")] = timestampMs;
    obj[QStringLiteral("wallClock")] = wallClock.toString(Qt::ISODateWithMs);
    obj[QStringLiteral("category")] = static_cast<int>(category);
    obj[QStringLiteral("severity")] = static_cast<int>(severity);
    obj[QStringLiteral("eventType")] = eventType;
    obj[QStringLiteral("summary")] = summary;
    obj[QStringLiteral("details")] = details;
    obj[QStringLiteral("data")] = data;
    obj[QStringLiteral("hmiState")] = static_cast<int>(hmiStateAtEvent);
    obj[QStringLiteral("automationLevel")] = static_cast<int>(levelAtEvent);
    obj[QStringLiteral("checksum")] = checksum;
    return obj;
}

HmiEvent HmiEvent::fromJson(const QJsonObject& json)
{
    HmiEvent event;
    event.sequenceId = static_cast<uint64_t>(json[QStringLiteral("sequenceId")].toInteger());
    event.timestampMs = json[QStringLiteral("timestampMs")].toInteger();
    event.wallClock = QDateTime::fromString(
        json[QStringLiteral("wallClock")].toString(), Qt::ISODateWithMs);
    event.category = static_cast<EventCategory>(json[QStringLiteral("category")].toInt());
    event.severity = static_cast<EventSeverity>(json[QStringLiteral("severity")].toInt());
    event.eventType = json[QStringLiteral("eventType")].toString();
    event.summary = json[QStringLiteral("summary")].toString();
    event.details = json[QStringLiteral("details")].toString();
    event.data = json[QStringLiteral("data")].toObject();
    event.hmiStateAtEvent = static_cast<AdasHmiState>(json[QStringLiteral("hmiState")].toInt());
    event.levelAtEvent = static_cast<AutomationLevel>(json[QStringLiteral("automationLevel")].toInt());
    event.checksum = json[QStringLiteral("checksum")].toString();
    return event;
}

HmiEventLog::HmiEventLog(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
    startNewSession();
}

HmiEventLog::~HmiEventLog() = default;

qint64 HmiEventLog::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

void HmiEventLog::logStateChange(AdasHmiState oldState, AdasHmiState newState,
                                  const QString& reason)
{
    QJsonObject data;
    data[QStringLiteral("oldState")] = static_cast<int>(oldState);
    data[QStringLiteral("newState")] = static_cast<int>(newState);

    logEvent(EventCategory::StateChange, EventSeverity::Info,
             QStringLiteral("state_change"),
             QString::fromLatin1("%1 -> %2")
                 .arg(adasHmiStateToString(oldState), adasHmiStateToString(newState)),
             reason, data);
}

void HmiEventLog::logEngagement(bool engaged, AutomationLevel level,
                                 const QString& reason)
{
    QJsonObject data;
    data[QStringLiteral("engaged")] = engaged;
    data[QStringLiteral("level")] = static_cast<int>(level);

    logEvent(EventCategory::Engagement,
             engaged ? EventSeverity::Info : EventSeverity::Info,
             engaged ? QStringLiteral("engaged") : QStringLiteral("standby"),
             QString::fromLatin1("%1 %2")
                 .arg(engaged ? QStringLiteral("Engaged") : QStringLiteral("Standby"),
                      automationLevelToString(level)),
             reason, data);
}

void HmiEventLog::logDisengagement(const QString& cause, const QJsonObject& context)
{
    logEvent(EventCategory::Engagement, EventSeverity::Warning,
             QStringLiteral("disengagement"),
             QStringLiteral("System disengaged"),
             cause, context);
}

void HmiEventLog::logTakeoverRequest(TakeoverUrgency urgency, const QString& reason)
{
    QJsonObject data;
    data[QStringLiteral("urgency")] = static_cast<int>(urgency);

    EventSeverity severity = EventSeverity::Warning;
    if (urgency >= TakeoverUrgency::Immediate) {
        severity = EventSeverity::Critical;
    }

    logEvent(EventCategory::Takeover, severity,
             QStringLiteral("takeover_request"),
             QStringLiteral("Takeover requested"),
             reason, data);
}

void HmiEventLog::logTakeoverResponse(bool acknowledged, double responseTimeSec)
{
    QJsonObject data;
    data[QStringLiteral("acknowledged")] = acknowledged;
    data[QStringLiteral("responseTimeSec")] = responseTimeSec;

    logEvent(EventCategory::Takeover,
             acknowledged ? EventSeverity::Info : EventSeverity::Error,
             acknowledged ? QStringLiteral("takeover_ack") : QStringLiteral("takeover_timeout"),
             acknowledged ? QStringLiteral("Driver took over")
                          : QStringLiteral("No driver response"),
             QString::fromLatin1("Response time: %1s").arg(responseTimeSec, 0, 'f', 1),
             data);
}

void HmiEventLog::logMrmEvent(const QString& reason, const QString& outcome)
{
    QJsonObject data;
    data[QStringLiteral("reason")] = reason;
    data[QStringLiteral("outcome")] = outcome;

    logEvent(EventCategory::Takeover, EventSeverity::Critical,
             QStringLiteral("mrm_event"),
             QStringLiteral("Minimal Risk Maneuver"),
             reason + QStringLiteral(" - ") + outcome, data);
}

void HmiEventLog::logPerceptionChange(ConfidenceLevel oldLevel, ConfidenceLevel newLevel,
                                       const QString& sensor)
{
    QJsonObject data;
    data[QStringLiteral("oldLevel")] = static_cast<int>(oldLevel);
    data[QStringLiteral("newLevel")] = static_cast<int>(newLevel);
    data[QStringLiteral("sensor")] = sensor;

    EventSeverity severity = EventSeverity::Info;
    if (newLevel < oldLevel) {
        severity = (newLevel <= ConfidenceLevel::Low)
                       ? EventSeverity::Warning : EventSeverity::Info;
    }

    logEvent(EventCategory::Perception, severity,
             QStringLiteral("perception_change"),
             QString::fromLatin1("%1 confidence changed").arg(sensor),
             QString(), data);
}

void HmiEventLog::logOddEvent(OddStatus status, const QString& reason)
{
    QJsonObject data;
    data[QStringLiteral("status")] = static_cast<int>(status);

    EventSeverity severity = EventSeverity::Info;
    if (status == OddStatus::Outside || status == OddStatus::ApproachingBoundary) {
        severity = EventSeverity::Warning;
    }

    logEvent(EventCategory::Odd, severity,
             QStringLiteral("odd_change"),
             QString::fromLatin1("ODD status: %1").arg(static_cast<int>(status)),
             reason, data);
}

void HmiEventLog::logDriverMonitoring(AttentionState state, const QString& details)
{
    QJsonObject data;
    data[QStringLiteral("attentionState")] = static_cast<int>(state);

    EventSeverity severity = EventSeverity::Info;
    if (state == AttentionState::Distracted || state == AttentionState::Drowsy) {
        severity = EventSeverity::Warning;
    } else if (state == AttentionState::Unresponsive) {
        severity = EventSeverity::Error;
    }

    logEvent(EventCategory::DriverMonitoring, severity,
             QStringLiteral("dms_event"),
             QString::fromLatin1("Driver attention: %1").arg(static_cast<int>(state)),
             details, data);
}

void HmiEventLog::logFault(const QString& faultCode, const QString& description,
                            EventSeverity severity)
{
    QJsonObject data;
    data[QStringLiteral("faultCode")] = faultCode;

    logEvent(EventCategory::Fault, severity,
             QStringLiteral("fault"),
             QString::fromLatin1("Fault: %1").arg(faultCode),
             description, data);
}

void HmiEventLog::logSecurityEvent(const QString& eventType, const QString& details)
{
    logEvent(EventCategory::Security, EventSeverity::Warning,
             eventType, QStringLiteral("Security event"), details);
}

void HmiEventLog::logUserAction(const QString& action, const QJsonObject& context)
{
    logEvent(EventCategory::UserAction, EventSeverity::Info,
             QStringLiteral("user_action"), action, QString(), context);
}

void HmiEventLog::logEvent(EventCategory category, EventSeverity severity,
                            const QString& eventType, const QString& summary,
                            const QString& details, const QJsonObject& data)
{
    HmiEvent event;
    event.timestampMs = currentMonotonicTimeMs();
    event.wallClock = QDateTime::currentDateTimeUtc();
    event.category = category;
    event.severity = severity;
    event.eventType = eventType;
    event.summary = summary;
    event.details = details;
    event.data = data;

    {
        QMutexLocker locker(&m_mutex);

        event.sequenceId = m_nextSequenceId++;
        event.hmiStateAtEvent = m_currentHmiState;
        event.levelAtEvent = m_currentLevel;
        event.checksum = computeEventChecksum(event);
        m_previousChecksum = event.checksum;

        if (severity == EventSeverity::Warning) {
            ++m_warningCount;
        } else if (severity >= EventSeverity::Error) {
            ++m_errorCount;
        }

        m_events.append(event);

        // Limit event storage
        while (m_events.size() > m_maxEvents) {
            m_events.removeFirst();
        }
    }

    emit eventsChanged();
    emit eventLogged(event);

    if (severity == EventSeverity::Warning) {
        emit warningLogged(event);
    } else if (severity >= EventSeverity::Error) {
        emit errorLogged(event);
    }
}

int HmiEventLog::eventCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_events.size();
}

bool HmiEventLog::hasWarnings() const
{
    QMutexLocker locker(&m_mutex);
    return m_warningCount > 0;
}

bool HmiEventLog::hasErrors() const
{
    QMutexLocker locker(&m_mutex);
    return m_errorCount > 0;
}

QVector<HmiEvent> HmiEventLog::recentEvents(int count) const
{
    QMutexLocker locker(&m_mutex);
    const qsizetype start = std::max(qsizetype{0}, m_events.size() - count);
    return m_events.mid(start);
}

QVector<HmiEvent> HmiEventLog::eventsByCategory(EventCategory category, int limit) const
{
    QMutexLocker locker(&m_mutex);
    QVector<HmiEvent> result;
    for (auto it = m_events.rbegin(); it != m_events.rend() && result.size() < limit; ++it) {
        if (it->category == category) {
            result.prepend(*it);
        }
    }
    return result;
}

QVector<HmiEvent> HmiEventLog::eventsInRange(qint64 startMs, qint64 endMs) const
{
    QMutexLocker locker(&m_mutex);
    QVector<HmiEvent> result;
    for (const auto& event : m_events) {
        if (event.timestampMs >= startMs && event.timestampMs <= endMs) {
            result.append(event);
        }
    }
    return result;
}

void HmiEventLog::startNewSession(const QString& sessionId)
{
    QMutexLocker locker(&m_mutex);
    m_sessionId = sessionId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces)
                                       : sessionId;
    m_sessionStartMs = currentMonotonicTimeMs();
    m_previousChecksum.clear();
    m_events.clear();
    m_nextSequenceId = 1;
    m_warningCount = 0;
    m_errorCount = 0;
}

QString HmiEventLog::currentSessionId() const
{
    QMutexLocker locker(&m_mutex);
    return m_sessionId;
}

qint64 HmiEventLog::sessionStartMs() const
{
    QMutexLocker locker(&m_mutex);
    return m_sessionStartMs;
}

QJsonObject HmiEventLog::exportToJson() const
{
    QMutexLocker locker(&m_mutex);

    QJsonArray eventsArray;
    for (const auto& event : m_events) {
        eventsArray.append(event.toJson());
    }

    QJsonObject root;
    root[QStringLiteral("sessionId")] = m_sessionId;
    root[QStringLiteral("sessionStartMs")] = m_sessionStartMs;
    root[QStringLiteral("eventCount")] = m_events.size();
    root[QStringLiteral("warningCount")] = m_warningCount;
    root[QStringLiteral("errorCount")] = m_errorCount;
    root[QStringLiteral("events")] = eventsArray;
    root[QStringLiteral("sessionChecksum")] = computeSessionChecksum();

    return root;
}

bool HmiEventLog::exportToFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(exportToJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

void HmiEventLog::setMaxEvents(int max)
{
    QMutexLocker locker(&m_mutex);
    m_maxEvents = max;
}

void HmiEventLog::setCurrentState(AdasHmiState state, AutomationLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_currentHmiState = state;
    m_currentLevel = level;
}

bool HmiEventLog::verifyIntegrity() const
{
    QMutexLocker locker(&m_mutex);

    QString previousHash;
    for (const auto& event : m_events) {
        // Recompute hash and verify
        HmiEvent copy = event;
        copy.checksum.clear();
        QString computed = computeEventChecksum(copy);

        if (computed != event.checksum) {
            return false;
        }
        previousHash = event.checksum;
    }
    return true;
}

QString HmiEventLog::computeSessionChecksum() const
{
    // Chain all event checksums
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(m_sessionId.toUtf8());

    for (const auto& event : m_events) {
        hash.addData(event.checksum.toUtf8());
    }

    return QString::fromLatin1(hash.result().toHex());
}

QString HmiEventLog::computeEventChecksum(const HmiEvent& event) const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);

    // Include previous checksum for chaining
    hash.addData(m_previousChecksum.toUtf8());

    // Include event data
    hash.addData(QByteArray::number(static_cast<qint64>(event.sequenceId)));
    hash.addData(QByteArray::number(event.timestampMs));
    hash.addData(event.eventType.toUtf8());
    hash.addData(event.summary.toUtf8());
    hash.addData(event.details.toUtf8());
    hash.addData(QJsonDocument(event.data).toJson(QJsonDocument::Compact));

    return QString::fromLatin1(hash.result().toHex().left(32));
}

} // namespace adas
} // namespace automotive
