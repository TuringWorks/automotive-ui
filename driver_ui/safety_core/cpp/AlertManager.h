// AlertManager.h
// Alert prioritization and management
// Safety: SR-CL-003 - Critical telltales shall remain visible

#ifndef AUTOMOTIVE_ALERT_MANAGER_H
#define AUTOMOTIVE_ALERT_MANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariantList>
#include <memory>

namespace automotive {
namespace driver {

/**
 * @brief Alert priority levels
 *
 * SR-CL-003: Strict prioritization, P0 cannot be occluded
 */
enum class AlertPriority : uint8_t {
    Critical = 0,   ///< P0 - Immediate danger, cannot be dismissed
    Warning = 1,    ///< P1 - Important warning, requires acknowledgment
    Info = 2,       ///< P2 - Informational
    Status = 3      ///< P3 - Status update
};

/**
 * @brief Alert data structure
 */
struct Alert {
    QString id;
    AlertPriority priority{AlertPriority::Info};
    QString title;
    QString message;
    QString actionText;
    bool dismissable{true};
    bool requiresAck{false};
    bool acknowledged{false};
    qint64 timestamp{0};
    qint64 expiresAt{0};  // 0 = no expiry

    bool isExpired(qint64 currentTime) const {
        return expiresAt > 0 && currentTime > expiresAt;
    }
};

/**
 * @brief Alert manager for cluster
 *
 * Manages alert queue with strict priority ordering.
 * Safety: SR-CL-003 - Critical alerts cannot be occluded or dismissed.
 */
class AlertManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(int alertCount READ alertCount NOTIFY alertCountChanged)
    Q_PROPERTY(bool hasAlerts READ hasAlerts NOTIFY hasAlertsChanged)
    Q_PROPERTY(bool hasCriticalAlerts READ hasCriticalAlerts NOTIFY hasCriticalAlertsChanged)
    Q_PROPERTY(QVariantList activeAlerts READ activeAlertsVariant NOTIFY activeAlertsChanged)

public:
    static constexpr int MAX_VISIBLE_ALERTS = 3;
    static constexpr int MAX_QUEUED_ALERTS = 20;

    explicit AlertManager(QObject* parent = nullptr);
    ~AlertManager() override;

    // Alert count
    int alertCount() const { return m_alerts.size(); }
    bool hasAlerts() const { return !m_alerts.isEmpty(); }
    bool hasCriticalAlerts() const;

    // Get active alerts (sorted by priority)
    QVector<Alert> activeAlerts() const;
    QVariantList activeAlertsVariant() const;

    /**
     * @brief Post a new alert
     * @param alert Alert to post
     * @return Alert ID
     */
    QString postAlert(const Alert& alert);

    /**
     * @brief Post alert with parameters
     */
    Q_INVOKABLE QString postAlert(int priority,
                                   const QString& title,
                                   const QString& message,
                                   bool dismissable = true,
                                   int durationMs = 0);

    /**
     * @brief Dismiss an alert
     * @param alertId Alert ID
     * @return true if dismissed (false if not dismissable)
     */
    Q_INVOKABLE bool dismissAlert(const QString& alertId);

    /**
     * @brief Acknowledge an alert
     * @param alertId Alert ID
     */
    Q_INVOKABLE void acknowledgeAlert(const QString& alertId);

    /**
     * @brief Clear all dismissable alerts
     */
    Q_INVOKABLE void clearDismissable();

    /**
     * @brief Clear all alerts (for testing only)
     */
    void clearAll();

    /**
     * @brief Process tick - check for expired alerts
     */
    void processTick(qint64 currentTimeMs);

signals:
    void alertCountChanged(int count);
    void hasAlertsChanged(bool hasAlerts);
    void hasCriticalAlertsChanged(bool hasCritical);
    void activeAlertsChanged();
    void alertPosted(const QString& alertId, int priority);
    void alertDismissed(const QString& alertId);
    void alertAcknowledged(const QString& alertId);

private:
    void sortAlerts();
    void emitChanges(bool countChanged, bool criticalChanged);
    QString generateAlertId() const;

    QVector<Alert> m_alerts;
    int m_alertIdCounter{0};
};

} // namespace driver
} // namespace automotive

Q_DECLARE_METATYPE(automotive::driver::AlertPriority)
Q_DECLARE_METATYPE(automotive::driver::Alert)

#endif // AUTOMOTIVE_ALERT_MANAGER_H
