// FaultInjector.h
// Fault injection for testing
// Safety: Used for verification of SR-CL-001..004

#ifndef AUTOMOTIVE_FAULT_INJECTOR_H
#define AUTOMOTIVE_FAULT_INJECTOR_H

#include <QObject>
#include <QTimer>
#include "signal/SignalHub.h"

namespace automotive {
namespace driver {

/**
 * @brief Fault injection scenarios
 */
enum class FaultScenario : uint8_t {
    None = 0,
    StaleSpeed,              ///< Speed signal goes stale (SR-CL-001)
    OutOfRangeSpeed,         ///< Speed exceeds valid range (SR-CL-002)
    InvalidGear,             ///< Gear signal invalid
    JitterySpeed,            ///< Rapid speed fluctuations
    MultipleStale,           ///< Multiple signals go stale (SR-CL-004)
    AllSignalsLost,          ///< All signals lost (degraded mode test)
    IntermittentSpeed,       ///< Speed comes and goes
    RateOfChangeViolation    ///< Speed changes too fast
};

/**
 * @brief Fault injector for testing safety requirements
 *
 * Provides controlled fault injection for verification testing.
 * Used to test:
 * - SR-CL-001: Speed freshness (stale within 300ms)
 * - SR-CL-002: Range validation and clamping
 * - SR-CL-003: Telltale visibility
 * - SR-CL-004: Degraded mode activation
 */
class FaultInjector : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(FaultScenario scenario READ scenario NOTIFY scenarioChanged)
    Q_PROPERTY(QString scenarioName READ scenarioName NOTIFY scenarioChanged)

public:
    explicit FaultInjector(signal::SignalHub* signalHub, QObject* parent = nullptr);
    ~FaultInjector() override;

    bool isActive() const { return m_active; }
    FaultScenario scenario() const { return m_scenario; }
    QString scenarioName() const;

    /**
     * @brief Start fault injection scenario
     * @param scenario Scenario to run
     * @param durationMs Duration (0 = indefinite)
     */
    Q_INVOKABLE void startScenario(int scenario, int durationMs = 0);

    /**
     * @brief Stop current scenario
     */
    Q_INVOKABLE void stopScenario();

    /**
     * @brief List available scenarios
     */
    Q_INVOKABLE QVariantList availableScenarios() const;

    /**
     * @brief Inject single fault
     */
    Q_INVOKABLE void injectStaleSpeed();
    Q_INVOKABLE void injectOutOfRangeSpeed(double speed);
    Q_INVOKABLE void injectInvalidGear();

signals:
    void activeChanged(bool active);
    void scenarioChanged(FaultScenario scenario);
    void faultInjected(const QString& description);
    void scenarioCompleted();

private slots:
    void onScenarioTick();
    void onScenarioTimeout();

private:
    void executeScenarioStep();

    signal::SignalHub* m_signalHub{nullptr};
    bool m_active{false};
    FaultScenario m_scenario{FaultScenario::None};
    QTimer m_scenarioTimer;
    QTimer m_durationTimer;
    int m_scenarioStep{0};
    double m_lastNormalSpeed{0.0};
};

} // namespace driver
} // namespace automotive

Q_DECLARE_METATYPE(automotive::driver::FaultScenario)

#endif // AUTOMOTIVE_FAULT_INJECTOR_H
