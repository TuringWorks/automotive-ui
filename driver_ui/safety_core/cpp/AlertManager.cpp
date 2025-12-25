// AlertManager.cpp
// Alert manager implementation

#include "AlertManager.h"
#include <QDateTime>
#include <algorithm>

namespace automotive {
namespace driver {

AlertManager::AlertManager(QObject* parent)
    : QObject(parent)
{
}

AlertManager::~AlertManager() = default;

bool AlertManager::hasCriticalAlerts() const
{
    for (const Alert& alert : m_alerts) {
        if (alert.priority == AlertPriority::Critical) {
            return true;
        }
    }
    return false;
}

QVector<Alert> AlertManager::activeAlerts() const
{
    // Return top N alerts (already sorted by priority)
    if (m_alerts.size() <= MAX_VISIBLE_ALERTS) {
        return m_alerts;
    }
    return m_alerts.mid(0, MAX_VISIBLE_ALERTS);
}

QVariantList AlertManager::activeAlertsVariant() const
{
    QVariantList result;
    QVector<Alert> alerts = activeAlerts();

    for (const Alert& alert : alerts) {
        QVariantMap map;
        map[QStringLiteral("id")] = alert.id;
        map[QStringLiteral("priority")] = static_cast<int>(alert.priority);
        map[QStringLiteral("title")] = alert.title;
        map[QStringLiteral("message")] = alert.message;
        map[QStringLiteral("actionText")] = alert.actionText;
        map[QStringLiteral("dismissable")] = alert.dismissable;
        map[QStringLiteral("requiresAck")] = alert.requiresAck;
        map[QStringLiteral("acknowledged")] = alert.acknowledged;
        result.append(map);
    }

    return result;
}

QString AlertManager::postAlert(const Alert& alert)
{
    // Check capacity
    if (m_alerts.size() >= MAX_QUEUED_ALERTS) {
        // Remove oldest non-critical alert
        for (int i = m_alerts.size() - 1; i >= 0; --i) {
            if (m_alerts[i].priority != AlertPriority::Critical) {
                m_alerts.removeAt(i);
                break;
            }
        }
    }

    Alert newAlert = alert;
    if (newAlert.id.isEmpty()) {
        newAlert.id = generateAlertId();
    }
    if (newAlert.timestamp == 0) {
        newAlert.timestamp = QDateTime::currentMSecsSinceEpoch();
    }

    // Critical alerts are never dismissable
    if (newAlert.priority == AlertPriority::Critical) {
        newAlert.dismissable = false;
        newAlert.requiresAck = true;
    }

    bool wasCritical = hasCriticalAlerts();
    int oldCount = m_alerts.size();

    m_alerts.append(newAlert);
    sortAlerts();

    emitChanges(m_alerts.size() != oldCount, hasCriticalAlerts() != wasCritical);
    emit alertPosted(newAlert.id, static_cast<int>(newAlert.priority));

    return newAlert.id;
}

QString AlertManager::postAlert(int priority,
                                 const QString& title,
                                 const QString& message,
                                 bool dismissable,
                                 int durationMs)
{
    Alert alert;
    alert.priority = static_cast<AlertPriority>(priority);
    alert.title = title;
    alert.message = message;
    alert.dismissable = dismissable;

    if (durationMs > 0) {
        alert.expiresAt = QDateTime::currentMSecsSinceEpoch() + durationMs;
    }

    return postAlert(alert);
}

bool AlertManager::dismissAlert(const QString& alertId)
{
    for (int i = 0; i < m_alerts.size(); ++i) {
        if (m_alerts[i].id == alertId) {
            // SR-CL-003: Critical alerts cannot be dismissed
            if (!m_alerts[i].dismissable) {
                return false;
            }

            bool wasCritical = hasCriticalAlerts();
            m_alerts.removeAt(i);

            emitChanges(true, hasCriticalAlerts() != wasCritical);
            emit alertDismissed(alertId);
            return true;
        }
    }
    return false;
}

void AlertManager::acknowledgeAlert(const QString& alertId)
{
    for (Alert& alert : m_alerts) {
        if (alert.id == alertId) {
            if (!alert.acknowledged) {
                alert.acknowledged = true;

                // Critical alerts become dismissable after acknowledgment
                if (alert.priority == AlertPriority::Critical) {
                    alert.dismissable = true;
                }

                emit alertAcknowledged(alertId);
                emit activeAlertsChanged();
            }
            return;
        }
    }
}

void AlertManager::clearDismissable()
{
    bool wasCritical = hasCriticalAlerts();
    int oldCount = m_alerts.size();

    m_alerts.erase(
        std::remove_if(m_alerts.begin(), m_alerts.end(),
            [](const Alert& a) { return a.dismissable; }),
        m_alerts.end()
    );

    if (m_alerts.size() != oldCount) {
        emitChanges(true, hasCriticalAlerts() != wasCritical);
    }
}

void AlertManager::clearAll()
{
    if (!m_alerts.isEmpty()) {
        m_alerts.clear();
        emitChanges(true, false);
    }
}

void AlertManager::processTick(qint64 currentTimeMs)
{
    bool changed = false;
    bool wasCritical = hasCriticalAlerts();

    for (int i = m_alerts.size() - 1; i >= 0; --i) {
        if (m_alerts[i].isExpired(currentTimeMs)) {
            // Don't auto-expire critical unacknowledged alerts
            if (m_alerts[i].priority == AlertPriority::Critical &&
                !m_alerts[i].acknowledged) {
                continue;
            }
            m_alerts.removeAt(i);
            changed = true;
        }
    }

    if (changed) {
        emitChanges(true, hasCriticalAlerts() != wasCritical);
    }
}

void AlertManager::sortAlerts()
{
    // Sort by priority (lower = higher priority), then by timestamp (newer first)
    std::stable_sort(m_alerts.begin(), m_alerts.end(),
        [](const Alert& a, const Alert& b) {
            if (a.priority != b.priority) {
                return static_cast<int>(a.priority) < static_cast<int>(b.priority);
            }
            return a.timestamp > b.timestamp;
        }
    );
}

void AlertManager::emitChanges(bool countChanged, bool criticalChanged)
{
    if (countChanged) {
        emit alertCountChanged(m_alerts.size());
        emit hasAlertsChanged(!m_alerts.isEmpty());
    }
    if (criticalChanged) {
        emit hasCriticalAlertsChanged(hasCriticalAlerts());
    }
    emit activeAlertsChanged();
}

QString AlertManager::generateAlertId() const
{
    return QString::fromLatin1("alert_%1_%2")
        .arg(++const_cast<AlertManager*>(this)->m_alertIdCounter)
        .arg(QDateTime::currentMSecsSinceEpoch());
}

} // namespace driver
} // namespace automotive
