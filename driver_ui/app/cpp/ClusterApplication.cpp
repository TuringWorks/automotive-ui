// ClusterApplication.cpp
// Cluster application implementation

#include "ClusterApplication.h"
#include "signal/VehicleSignals.h"
#include <QTimer>
#include <QRandomGenerator>
#include <cmath>

namespace automotive {
namespace driver {

ClusterApplication::ClusterApplication(signal::SignalHub* signalHub,
                                       sched::DeterministicScheduler* scheduler,
                                       QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
    , m_scheduler(scheduler)
{
    Q_ASSERT(signalHub != nullptr);
    Q_ASSERT(scheduler != nullptr);

    // Create components
    m_stateModel = std::make_unique<ClusterStateModel>(signalHub, this);
    m_alertManager = std::make_unique<AlertManager>(this);
    m_telltaleManager = std::make_unique<TelltaleManager>(signalHub, this);
    m_degradedController = std::make_unique<DegradedModeController>(signalHub, this);
    m_safetyMonitor = std::make_unique<SafetyMonitor>(signalHub, scheduler, this);
    m_faultInjector = std::make_unique<FaultInjector>(signalHub, this);

    // Initialize telltales
    m_telltaleManager->initializeDefaults();

    // Connect scheduler tick
    connect(m_scheduler, &sched::DeterministicScheduler::tick,
            this, &ClusterApplication::onSchedulerTick);

    // Create simulation timer
    m_simTimer = new QTimer(this);
    m_simTimer->setInterval(50);  // 20Hz simulation
    connect(m_simTimer, &QTimer::timeout,
            this, &ClusterApplication::onSimulationTick);
}

ClusterApplication::~ClusterApplication()
{
    stop();
}

void ClusterApplication::start()
{
    if (m_running) {
        return;
    }

    // Start scheduler at 20Hz for signal processing
    m_scheduler->start(sched::DeterministicScheduler::SIGNAL_TICK_HZ);

    m_running = true;
    emit runningChanged(true);

    // Auto-start simulation for desktop
    startSimulation();
}

void ClusterApplication::stop()
{
    if (!m_running) {
        return;
    }

    stopSimulation();
    m_scheduler->stop();

    m_running = false;
    emit runningChanged(false);
}

void ClusterApplication::startSimulation()
{
    if (m_simulating) {
        return;
    }

    // Initialize simulation values
    m_simSpeed = 0.0;
    m_simBattery = 75.0;

    // Set initial gear
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::GEAR_POSITION),
        QStringLiteral("P"));

    // Set initial battery
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::BATTERY_SOC),
        m_simBattery);
    m_signalHub->updateSignal(
        QString::fromLatin1(signal::SignalIds::BATTERY_RANGE),
        m_simBattery * 4.0);  // ~4km per %

    m_simTimer->start();
    m_simulating = true;
    emit simulatingChanged(true);
}

void ClusterApplication::stopSimulation()
{
    if (!m_simulating) {
        return;
    }

    m_simTimer->stop();
    m_simulating = false;
    emit simulatingChanged(false);
}

void ClusterApplication::onSchedulerTick(uint64_t tickNumber, qint64 elapsedMs)
{
    // Process state model
    m_stateModel->processTick(tickNumber, elapsedMs);

    // Process alerts
    m_alertManager->processTick(elapsedMs);

    // Process degraded mode
    m_degradedController->processTick(elapsedMs);

    // Process safety monitor
    m_safetyMonitor->processTick(elapsedMs);
}

void ClusterApplication::onSimulationTick()
{
    if (!m_simulating || m_faultInjector->isActive()) {
        return;
    }

    using namespace signal;

    // Simulate speed changes
    static int simTick = 0;
    simTick++;

    // Generate smooth speed curve
    double targetSpeed = 60.0 + 40.0 * std::sin(simTick * 0.02);
    targetSpeed = qBound(0.0, targetSpeed, 140.0);

    // Smooth approach to target (reduced rate to avoid rate-of-change violations)
    double speedDiff = targetSpeed - m_simSpeed;
    m_simSpeed += speedDiff * 0.02;  // Slower, smoother changes
    m_simSpeed = qBound(0.0, m_simSpeed, 200.0);

    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::VEHICLE_SPEED), m_simSpeed);

    // Simulate gear based on speed
    QString gear;
    if (m_simSpeed < 1.0) {
        gear = QStringLiteral("P");
    } else if (m_simSpeed < 30.0) {
        gear = QStringLiteral("D");  // Low speed
    } else {
        gear = QStringLiteral("D");  // Normal drive
    }
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::GEAR_POSITION), gear);

    // Simulate battery drain
    if (simTick % 100 == 0 && m_simBattery > 10.0) {
        m_simBattery -= 0.1;
        m_signalHub->updateSignal(
            QString::fromLatin1(SignalIds::BATTERY_SOC), m_simBattery);
        m_signalHub->updateSignal(
            QString::fromLatin1(SignalIds::BATTERY_RANGE), m_simBattery * 4.0);
    }

    // Simulate power consumption
    double power = m_simSpeed * 0.5 + QRandomGenerator::global()->bounded(10);
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::POWER_CONSUMPTION), power);

    // Simulate turn signals - alternating left/right every 8 seconds
    bool leftTurn = (simTick / 160) % 2 == 0 && (simTick % 160) < 80;
    bool rightTurn = (simTick / 160) % 2 == 1 && (simTick % 160) < 80;
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TURN_LEFT), leftTurn);
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TURN_RIGHT), rightTurn);

    // Debug output every 2 seconds
    if (simTick % 40 == 0) {
        qDebug() << "Turn signals - Left:" << leftTurn << "Right:" << rightTurn
                 << "Active telltales:" << m_telltaleManager->activeCount();
    }

    // Simulate low beam
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::TELLTALE_LOW_BEAM), true);

    // Simulate outside temperature
    m_signalHub->updateSignal(
        QString::fromLatin1(SignalIds::OUTSIDE_TEMP), 22.0);
}

} // namespace driver
} // namespace automotive
