// ClusterViewModel.cpp
// Cluster view model implementation

#include "ClusterViewModel.h"

namespace automotive {
namespace driver {

ClusterViewModel::ClusterViewModel(ClusterStateModel* stateModel,
                                   AlertManager* alertManager,
                                   TelltaleManager* telltaleManager,
                                   DegradedModeController* degradedController,
                                   QObject* parent)
    : QObject(parent)
    , m_stateModel(stateModel)
    , m_alertManager(alertManager)
    , m_telltaleManager(telltaleManager)
    , m_degradedController(degradedController)
{
    Q_ASSERT(stateModel != nullptr);
    Q_ASSERT(alertManager != nullptr);
    Q_ASSERT(telltaleManager != nullptr);
    Q_ASSERT(degradedController != nullptr);

    // Forward state model signals
    connect(m_stateModel, &ClusterStateModel::speedChanged,
            this, &ClusterViewModel::speedChanged);
    connect(m_stateModel, &ClusterStateModel::speedDisplayChanged,
            this, &ClusterViewModel::speedDisplayChanged);
    connect(m_stateModel, &ClusterStateModel::speedValidChanged,
            this, &ClusterViewModel::speedValidChanged);
    connect(m_stateModel, &ClusterStateModel::speedStaleChanged,
            this, &ClusterViewModel::speedStaleChanged);
    connect(m_stateModel, &ClusterStateModel::speedUnitChanged,
            this, &ClusterViewModel::speedUnitChanged);

    connect(m_stateModel, &ClusterStateModel::gearChanged,
            this, &ClusterViewModel::gearChanged);
    connect(m_stateModel, &ClusterStateModel::gearValidChanged,
            this, &ClusterViewModel::gearValidChanged);

    connect(m_stateModel, &ClusterStateModel::batteryLevelChanged,
            this, &ClusterViewModel::batteryLevelChanged);
    connect(m_stateModel, &ClusterStateModel::rangeChanged,
            this, &ClusterViewModel::rangeChanged);
    connect(m_stateModel, &ClusterStateModel::powerConsumptionChanged,
            this, &ClusterViewModel::powerChanged);
    connect(m_stateModel, &ClusterStateModel::batteryValidChanged,
            this, &ClusterViewModel::batteryValidChanged);

    connect(m_stateModel, &ClusterStateModel::isDegradedChanged,
            this, &ClusterViewModel::isDegradedChanged);
    connect(m_stateModel, &ClusterStateModel::invalidSignalCountChanged,
            this, &ClusterViewModel::invalidSignalCountChanged);

    connect(m_stateModel, &ClusterStateModel::timeDisplayChanged,
            this, &ClusterViewModel::timeDisplayChanged);
    connect(m_stateModel, &ClusterStateModel::outsideTempChanged,
            this, &ClusterViewModel::outsideTempChanged);

    // Forward alert manager signals
    connect(m_alertManager, &AlertManager::hasAlertsChanged,
            this, &ClusterViewModel::hasAlertsChanged);
    connect(m_alertManager, &AlertManager::hasCriticalAlertsChanged,
            this, &ClusterViewModel::hasCriticalAlertsChanged);
    connect(m_alertManager, &AlertManager::activeAlertsChanged,
            this, &ClusterViewModel::alertsChanged);

    // Forward telltale manager signals
    connect(m_telltaleManager, &TelltaleManager::activeTelltalesChanged,
            this, &ClusterViewModel::telltalesChanged);
    connect(m_telltaleManager, &TelltaleManager::hasCriticalChanged,
            this, &ClusterViewModel::hasCriticalTelltalesChanged);

    // Forward degraded controller signals
    connect(m_degradedController, &DegradedModeController::degradedMessageChanged,
            this, &ClusterViewModel::degradedMessageChanged);
    connect(m_degradedController, &DegradedModeController::visibilityChanged,
            this, &ClusterViewModel::visibilityChanged);
}

ClusterViewModel::~ClusterViewModel() = default;

QString ClusterViewModel::speedDisplay() const
{
    return m_stateModel->speedDisplay();
}

void ClusterViewModel::dismissAlert(const QString& alertId)
{
    m_alertManager->dismissAlert(alertId);
}

void ClusterViewModel::acknowledgeAlert(const QString& alertId)
{
    m_alertManager->acknowledgeAlert(alertId);
}

} // namespace driver
} // namespace automotive
