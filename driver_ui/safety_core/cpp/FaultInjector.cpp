// FaultInjector.cpp
// Fault injector implementation

#include "FaultInjector.h"
#include "signal/VehicleSignals.h"
#include <QRandomGenerator>

namespace automotive {
namespace driver {

FaultInjector::FaultInjector(signal::SignalHub* signalHub, QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
{
    Q_ASSERT(signalHub != nullptr);

    connect(&m_scenarioTimer, &QTimer::timeout,
            this, &FaultInjector::onScenarioTick);
    connect(&m_durationTimer, &QTimer::timeout,
            this, &FaultInjector::onScenarioTimeout);

    m_durationTimer.setSingleShot(true);
}

FaultInjector::~FaultInjector()
{
    stopScenario();
}

QString FaultInjector::scenarioName() const
{
    switch (m_scenario) {
    case FaultScenario::None: return QStringLiteral("None");
    case FaultScenario::StaleSpeed: return QStringLiteral("Stale Speed");
    case FaultScenario::OutOfRangeSpeed: return QStringLiteral("Out of Range Speed");
    case FaultScenario::InvalidGear: return QStringLiteral("Invalid Gear");
    case FaultScenario::JitterySpeed: return QStringLiteral("Jittery Speed");
    case FaultScenario::MultipleStale: return QStringLiteral("Multiple Stale");
    case FaultScenario::AllSignalsLost: return QStringLiteral("All Signals Lost");
    case FaultScenario::IntermittentSpeed: return QStringLiteral("Intermittent Speed");
    case FaultScenario::RateOfChangeViolation: return QStringLiteral("Rate of Change Violation");
    }
    return QStringLiteral("Unknown");
}

void FaultInjector::startScenario(int scenario, int durationMs)
{
    if (m_active) {
        stopScenario();
    }

    m_scenario = static_cast<FaultScenario>(scenario);
    m_scenarioStep = 0;
    m_active = true;

    // Save current speed for restoration
    auto speedSignal = m_signalHub->getSignal(
        QString::fromLatin1(signal::SignalIds::VEHICLE_SPEED));
    m_lastNormalSpeed = speedSignal.value.toDouble();

    // Start scenario timer
    m_scenarioTimer.start(100);  // 10Hz scenario updates

    if (durationMs > 0) {
        m_durationTimer.start(durationMs);
    }

    emit activeChanged(true);
    emit scenarioChanged(m_scenario);
    emit faultInjected(QString::fromLatin1("Started scenario: %1").arg(scenarioName()));
}

void FaultInjector::stopScenario()
{
    if (!m_active) {
        return;
    }

    m_scenarioTimer.stop();
    m_durationTimer.stop();

    // Restore normal signal updates
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::VEHICLE_SPEED),
        m_lastNormalSpeed);
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::GEAR_POSITION),
        QStringLiteral("D"));

    m_active = false;
    m_scenario = FaultScenario::None;

    emit activeChanged(false);
    emit scenarioChanged(m_scenario);
    emit scenarioCompleted();
}

QVariantList FaultInjector::availableScenarios() const
{
    QVariantList list;

    auto addScenario = [&list](int id, const QString& name, const QString& desc) {
        QVariantMap map;
        map[QStringLiteral("id")] = id;
        map[QStringLiteral("name")] = name;
        map[QStringLiteral("description")] = desc;
        list.append(map);
    };

    addScenario(1, QStringLiteral("Stale Speed"),
                QStringLiteral("Speed signal stops updating (tests SR-CL-001)"));
    addScenario(2, QStringLiteral("Out of Range Speed"),
                QStringLiteral("Speed exceeds valid range (tests SR-CL-002)"));
    addScenario(3, QStringLiteral("Invalid Gear"),
                QStringLiteral("Gear signal becomes invalid"));
    addScenario(4, QStringLiteral("Jittery Speed"),
                QStringLiteral("Rapid speed fluctuations"));
    addScenario(5, QStringLiteral("Multiple Stale"),
                QStringLiteral("Multiple signals go stale (tests SR-CL-004)"));
    addScenario(6, QStringLiteral("All Signals Lost"),
                QStringLiteral("All signals lost (tests degraded mode)"));
    addScenario(7, QStringLiteral("Intermittent Speed"),
                QStringLiteral("Speed comes and goes"));
    addScenario(8, QStringLiteral("Rate of Change Violation"),
                QStringLiteral("Speed changes faster than allowed"));

    return list;
}

void FaultInjector::injectStaleSpeed()
{
    // Simply don't update the speed signal - it will go stale naturally
    emit faultInjected(QStringLiteral("Injected stale speed"));
}

void FaultInjector::injectOutOfRangeSpeed(double speed)
{
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::VEHICLE_SPEED),
        speed);  // Will be clamped by validation
    emit faultInjected(QString::fromLatin1("Injected out-of-range speed: %1").arg(speed));
}

void FaultInjector::injectInvalidGear()
{
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::GEAR_POSITION),
        QStringLiteral("X"));  // Invalid gear
    emit faultInjected(QStringLiteral("Injected invalid gear"));
}

void FaultInjector::onScenarioTick()
{
    executeScenarioStep();
    m_scenarioStep++;
}

void FaultInjector::onScenarioTimeout()
{
    stopScenario();
}

void FaultInjector::executeScenarioStep()
{
    using namespace signal;

    switch (m_scenario) {
    case FaultScenario::StaleSpeed:
        // Don't update speed - let it go stale
        break;

    case FaultScenario::OutOfRangeSpeed:
        // Inject out-of-range speed
        m_signalHub->updateSignal(
            QString::fromLatin1(SignalIds::VEHICLE_SPEED), 500.0);
        break;

    case FaultScenario::InvalidGear:
        m_signalHub->updateSignal(
            QString::fromLatin1(SignalIds::GEAR_POSITION), QStringLiteral("?"));
        break;

    case FaultScenario::JitterySpeed:
        // Rapid fluctuations
        {
            double jitter = QRandomGenerator::global()->bounded(50) - 25;
            m_signalHub->updateSignal(
                QString::fromLatin1(SignalIds::VEHICLE_SPEED),
                m_lastNormalSpeed + jitter);
        }
        break;

    case FaultScenario::MultipleStale:
        // Don't update any signals
        break;

    case FaultScenario::AllSignalsLost:
        // Don't update any signals
        break;

    case FaultScenario::IntermittentSpeed:
        // Update every other tick
        if (m_scenarioStep % 2 == 0) {
            m_signalHub->updateSignal(
                QString::fromLatin1(SignalIds::VEHICLE_SPEED),
                m_lastNormalSpeed);
        }
        break;

    case FaultScenario::RateOfChangeViolation:
        // Jump speed dramatically
        {
            double speed = (m_scenarioStep % 2 == 0) ? 0.0 : 200.0;
            m_signalHub->updateSignal(
                QString::fromLatin1(SignalIds::VEHICLE_SPEED), speed);
        }
        break;

    case FaultScenario::None:
        break;
    }
}

} // namespace driver
} // namespace automotive
