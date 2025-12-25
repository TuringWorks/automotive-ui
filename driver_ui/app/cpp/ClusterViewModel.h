// ClusterViewModel.h
// View model for QML bindings
// Safety: Exposes validated data to QML (display only)

#ifndef AUTOMOTIVE_CLUSTER_VIEW_MODEL_H
#define AUTOMOTIVE_CLUSTER_VIEW_MODEL_H

#include <QObject>
#include "ClusterStateModel.h"
#include "AlertManager.h"
#include "TelltaleManager.h"
#include "DegradedModeController.h"

namespace automotive {
namespace driver {

/**
 * @brief View model for cluster QML
 *
 * Provides a clean interface between C++ models and QML views.
 * All data is already validated by the time it reaches QML.
 */
class ClusterViewModel : public QObject {
    Q_OBJECT

    // Speed
    Q_PROPERTY(QString speedDisplay READ speedDisplay NOTIFY speedDisplayChanged)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(double speedPercent READ speedPercent NOTIFY speedChanged)
    Q_PROPERTY(bool speedValid READ speedValid NOTIFY speedValidChanged)
    Q_PROPERTY(bool speedStale READ speedStale NOTIFY speedStaleChanged)
    Q_PROPERTY(QString speedUnit READ speedUnit NOTIFY speedUnitChanged)

    // Gear
    Q_PROPERTY(QString gear READ gear NOTIFY gearChanged)
    Q_PROPERTY(bool gearValid READ gearValid NOTIFY gearValidChanged)

    // Energy
    Q_PROPERTY(double batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(double range READ range NOTIFY rangeChanged)
    Q_PROPERTY(double power READ power NOTIFY powerChanged)
    Q_PROPERTY(bool batteryValid READ batteryValid NOTIFY batteryValidChanged)

    // State
    Q_PROPERTY(bool isDegraded READ isDegraded NOTIFY isDegradedChanged)
    Q_PROPERTY(QString degradedMessage READ degradedMessage NOTIFY degradedMessageChanged)
    Q_PROPERTY(int invalidSignalCount READ invalidSignalCount NOTIFY invalidSignalCountChanged)

    // Environment
    Q_PROPERTY(QString timeDisplay READ timeDisplay NOTIFY timeDisplayChanged)
    Q_PROPERTY(double outsideTemp READ outsideTemp NOTIFY outsideTempChanged)

    // Alerts
    Q_PROPERTY(bool hasAlerts READ hasAlerts NOTIFY hasAlertsChanged)
    Q_PROPERTY(bool hasCriticalAlerts READ hasCriticalAlerts NOTIFY hasCriticalAlertsChanged)
    Q_PROPERTY(QVariantList alerts READ alerts NOTIFY alertsChanged)

    // Telltales
    Q_PROPERTY(QVariantList activeTelltales READ activeTelltales NOTIFY telltalesChanged)
    Q_PROPERTY(QVariantList criticalTelltales READ criticalTelltales NOTIFY telltalesChanged)
    Q_PROPERTY(bool hasCriticalTelltales READ hasCriticalTelltales NOTIFY hasCriticalTelltalesChanged)

    // Visibility (from degraded controller)
    Q_PROPERTY(bool showSecondaryGauges READ showSecondaryGauges NOTIFY visibilityChanged)
    Q_PROPERTY(bool showInfoPanel READ showInfoPanel NOTIFY visibilityChanged)

public:
    explicit ClusterViewModel(ClusterStateModel* stateModel,
                              AlertManager* alertManager,
                              TelltaleManager* telltaleManager,
                              DegradedModeController* degradedController,
                              QObject* parent = nullptr);
    ~ClusterViewModel() override;

    // Speed
    QString speedDisplay() const;
    double speed() const { return m_stateModel->speed(); }
    double speedPercent() const { return m_stateModel->speed() / 200.0; }  // Normalized 0-1
    bool speedValid() const { return m_stateModel->speedValid(); }
    bool speedStale() const { return m_stateModel->speedStale(); }
    QString speedUnit() const { return m_stateModel->speedUnit(); }

    // Gear
    QString gear() const { return m_stateModel->gear(); }
    bool gearValid() const { return m_stateModel->gearValid(); }

    // Energy
    double batteryLevel() const { return m_stateModel->batteryLevel(); }
    double range() const { return m_stateModel->range(); }
    double power() const { return m_stateModel->powerConsumption(); }
    bool batteryValid() const { return m_stateModel->batteryValid(); }

    // State
    bool isDegraded() const { return m_stateModel->isDegraded(); }
    QString degradedMessage() const { return m_degradedController->degradedMessage(); }
    int invalidSignalCount() const { return m_stateModel->invalidSignalCount(); }

    // Environment
    QString timeDisplay() const { return m_stateModel->timeDisplay(); }
    double outsideTemp() const { return m_stateModel->outsideTemp(); }

    // Alerts
    bool hasAlerts() const { return m_alertManager->hasAlerts(); }
    bool hasCriticalAlerts() const { return m_alertManager->hasCriticalAlerts(); }
    QVariantList alerts() const { return m_alertManager->activeAlertsVariant(); }

    // Telltales
    QVariantList activeTelltales() const { return m_telltaleManager->activeTelltalesToVariant(); }
    QVariantList criticalTelltales() const { return m_telltaleManager->criticalTelltalesToVariant(); }
    bool hasCriticalTelltales() const { return m_telltaleManager->hasCritical(); }

    // Visibility
    bool showSecondaryGauges() const { return m_degradedController->showSecondaryGauges(); }
    bool showInfoPanel() const { return m_degradedController->showInfoPanel(); }

    // Actions
    Q_INVOKABLE void dismissAlert(const QString& alertId);
    Q_INVOKABLE void acknowledgeAlert(const QString& alertId);

signals:
    // Speed
    void speedDisplayChanged();
    void speedChanged();
    void speedValidChanged();
    void speedStaleChanged();
    void speedUnitChanged();

    // Gear
    void gearChanged();
    void gearValidChanged();

    // Energy
    void batteryLevelChanged();
    void rangeChanged();
    void powerChanged();
    void batteryValidChanged();

    // State
    void isDegradedChanged();
    void degradedMessageChanged();
    void invalidSignalCountChanged();

    // Environment
    void timeDisplayChanged();
    void outsideTempChanged();

    // Alerts
    void hasAlertsChanged();
    void hasCriticalAlertsChanged();
    void alertsChanged();

    // Telltales
    void telltalesChanged();
    void hasCriticalTelltalesChanged();

    // Visibility
    void visibilityChanged();

private:
    ClusterStateModel* m_stateModel{nullptr};
    AlertManager* m_alertManager{nullptr};
    TelltaleManager* m_telltaleManager{nullptr};
    DegradedModeController* m_degradedController{nullptr};
};

} // namespace driver
} // namespace automotive

#endif // AUTOMOTIVE_CLUSTER_VIEW_MODEL_H
