// ClusterApplication.h
// Main cluster application coordinator
// Safety: Coordinates all safety-relevant components

#ifndef AUTOMOTIVE_CLUSTER_APPLICATION_H
#define AUTOMOTIVE_CLUSTER_APPLICATION_H

#include <QObject>
#include <memory>

#include "signal/SignalHub.h"
#include "sched/DeterministicScheduler.h"
#include "ClusterStateModel.h"
#include "AlertManager.h"
#include "TelltaleManager.h"
#include "DegradedModeController.h"
#include "SafetyMonitor.h"
#include "FaultInjector.h"

namespace automotive {
namespace driver {

/**
 * @brief Main cluster application
 *
 * Coordinates all cluster components:
 * - Signal processing
 * - State management
 * - Alert handling
 * - Telltale management
 * - Degraded mode control
 * - Safety monitoring
 */
class ClusterApplication : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool simulating READ isSimulating NOTIFY simulatingChanged)

public:
    explicit ClusterApplication(signal::SignalHub* signalHub,
                                sched::DeterministicScheduler* scheduler,
                                QObject* parent = nullptr);
    ~ClusterApplication() override;

    // Component access
    ClusterStateModel* stateModel() { return m_stateModel.get(); }
    AlertManager* alertManager() { return m_alertManager.get(); }
    TelltaleManager* telltaleManager() { return m_telltaleManager.get(); }
    DegradedModeController* degradedController() { return m_degradedController.get(); }
    SafetyMonitor* safetyMonitor() { return m_safetyMonitor.get(); }
    FaultInjector* faultInjector() { return m_faultInjector.get(); }

    bool isRunning() const { return m_running; }
    bool isSimulating() const { return m_simulating; }

public slots:
    /**
     * @brief Start the cluster application
     */
    void start();

    /**
     * @brief Stop the cluster application
     */
    void stop();

    /**
     * @brief Start simulation mode (for desktop testing)
     */
    void startSimulation();

    /**
     * @brief Stop simulation mode
     */
    void stopSimulation();

signals:
    void runningChanged(bool running);
    void simulatingChanged(bool simulating);

private slots:
    void onSchedulerTick(uint64_t tickNumber, qint64 elapsedMs);
    void onSimulationTick();

private:
    signal::SignalHub* m_signalHub{nullptr};
    sched::DeterministicScheduler* m_scheduler{nullptr};

    std::unique_ptr<ClusterStateModel> m_stateModel;
    std::unique_ptr<AlertManager> m_alertManager;
    std::unique_ptr<TelltaleManager> m_telltaleManager;
    std::unique_ptr<DegradedModeController> m_degradedController;
    std::unique_ptr<SafetyMonitor> m_safetyMonitor;
    std::unique_ptr<FaultInjector> m_faultInjector;

    QTimer* m_simTimer{nullptr};
    bool m_running{false};
    bool m_simulating{false};
    double m_simSpeed{0.0};
    double m_simBattery{75.0};
};

} // namespace driver
} // namespace automotive

#endif // AUTOMOTIVE_CLUSTER_APPLICATION_H
