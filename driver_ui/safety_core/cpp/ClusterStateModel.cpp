// ClusterStateModel.cpp
// Central state model implementation

#include "ClusterStateModel.h"
#include "signal/VehicleSignals.h"
#include <QDateTime>

namespace automotive {
namespace driver {

ClusterStateModel::ClusterStateModel(signal::SignalHub* signalHub, QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
{
    Q_ASSERT(signalHub != nullptr);

    connect(m_signalHub, &signal::SignalHub::signalUpdated,
            this, &ClusterStateModel::onSignalUpdated);
    connect(m_signalHub, &signal::SignalHub::degradedModeChanged,
            this, &ClusterStateModel::onDegradedModeChanged);
}

ClusterStateModel::~ClusterStateModel() = default;

QString ClusterStateModel::speedDisplay() const
{
    if (!m_speedValid) {
        return QStringLiteral("—");
    }
    if (m_speedStale) {
        return QStringLiteral("—");  // Could also show last value with indicator
    }
    return QString::number(static_cast<int>(m_speed));
}

void ClusterStateModel::processTick(uint64_t tickNumber, qint64 elapsedMs)
{
    Q_UNUSED(tickNumber)
    Q_UNUSED(elapsedMs)

    // Update time display
    QString newTime = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm"));
    if (newTime != m_timeDisplay) {
        m_timeDisplay = newTime;
        emit timeDisplayChanged(m_timeDisplay);
    }

    // Check for stale signals
    m_signalHub->checkFreshness();
}

void ClusterStateModel::forceDegradedMode(bool degraded)
{
    m_forcedDegraded = degraded;
    updateClusterState();
}

void ClusterStateModel::onSignalUpdated(const QString& signalId,
                                         const signal::SignalValue& value)
{
    using namespace signal;

    // Speed update (SR-CL-001)
    if (signalId == QString::fromLatin1(SignalIds::VEHICLE_SPEED)) {
        double newSpeed = value.value.toDouble();
        bool newValid = value.isValid();
        bool newStale = value.validity == SignalValidity::Stale;

        if (newSpeed != m_speed) {
            m_speed = newSpeed;
            emit speedChanged(m_speed);
            emit speedDisplayChanged(speedDisplay());
        }
        if (newValid != m_speedValid) {
            m_speedValid = newValid;
            emit speedValidChanged(m_speedValid);
            emit speedDisplayChanged(speedDisplay());
        }
        if (newStale != m_speedStale) {
            m_speedStale = newStale;
            emit speedStaleChanged(m_speedStale);
            emit speedDisplayChanged(speedDisplay());
        }
    }
    // Gear update
    else if (signalId == QString::fromLatin1(SignalIds::GEAR_POSITION)) {
        QString newGear = value.value.toString().toUpper();
        bool newValid = value.isValid();

        if (newGear != m_gear) {
            m_gear = newGear;
            emit gearChanged(m_gear);

            DriveMode newMode = gearToDriveMode(m_gear);
            if (newMode != m_driveMode) {
                m_driveMode = newMode;
                emit driveModeChanged(m_driveMode);
            }
        }
        if (newValid != m_gearValid) {
            m_gearValid = newValid;
            emit gearValidChanged(m_gearValid);
        }
    }
    // Battery SOC
    else if (signalId == QString::fromLatin1(SignalIds::BATTERY_SOC)) {
        double newLevel = value.value.toDouble();
        bool newValid = value.isValid();

        if (newLevel != m_batteryLevel) {
            m_batteryLevel = newLevel;
            emit batteryLevelChanged(m_batteryLevel);
        }
        if (newValid != m_batteryValid) {
            m_batteryValid = newValid;
            emit batteryValidChanged(m_batteryValid);
        }
    }
    // Range
    else if (signalId == QString::fromLatin1(SignalIds::BATTERY_RANGE)) {
        double newRange = value.value.toDouble();
        bool newValid = value.isValid();

        if (newRange != m_range) {
            m_range = newRange;
            emit rangeChanged(m_range);
        }
        if (newValid != m_rangeValid) {
            m_rangeValid = newValid;
            emit rangeValidChanged(m_rangeValid);
        }
    }
    // Power consumption
    else if (signalId == QString::fromLatin1(SignalIds::POWER_CONSUMPTION)) {
        double newPower = value.value.toDouble();
        if (newPower != m_powerConsumption) {
            m_powerConsumption = newPower;
            emit powerConsumptionChanged(m_powerConsumption);
        }
    }
    // Outside temperature
    else if (signalId == QString::fromLatin1(SignalIds::OUTSIDE_TEMP)) {
        double newTemp = value.value.toDouble();
        if (newTemp != m_outsideTemp) {
            m_outsideTemp = newTemp;
            emit outsideTempChanged(m_outsideTemp);
        }
    }

    // Update overall cluster state
    updateClusterState();
}

void ClusterStateModel::onDegradedModeChanged(bool degraded)
{
    Q_UNUSED(degraded)
    updateClusterState();
}

void ClusterStateModel::updateClusterState()
{
    ClusterState newState = ClusterState::Normal;

    // Check forced degraded mode (testing)
    if (m_forcedDegraded) {
        newState = ClusterState::Degraded;
    }
    // Check signal hub degraded mode (SR-CL-004)
    else if (m_signalHub->isDegradedMode()) {
        newState = ClusterState::Degraded;
    }
    // Check for warnings
    else if (!m_speedValid || !m_gearValid) {
        newState = ClusterState::Warning;
    }

    // Update invalid signal count
    int newInvalidCount = m_signalHub->invalidSignalCount();
    if (newInvalidCount != m_invalidSignalCount) {
        m_invalidSignalCount = newInvalidCount;
        emit invalidSignalCountChanged(m_invalidSignalCount);
    }

    // Update cluster state
    if (newState != m_clusterState) {
        bool wasDegraded = isDegraded();
        m_clusterState = newState;
        emit clusterStateChanged(m_clusterState);

        if (isDegraded() != wasDegraded) {
            emit isDegradedChanged(isDegraded());
        }
    }
}

DriveMode ClusterStateModel::gearToDriveMode(const QString& gear) const
{
    if (gear == QLatin1String("P")) return DriveMode::Park;
    if (gear == QLatin1String("R")) return DriveMode::Reverse;
    if (gear == QLatin1String("N")) return DriveMode::Neutral;
    if (gear == QLatin1String("D")) return DriveMode::Drive;
    if (gear == QLatin1String("S")) return DriveMode::Sport;
    if (gear == QLatin1String("L") || gear == QLatin1String("B")) return DriveMode::Eco;
    if (gear == QLatin1String("M")) return DriveMode::Manual;
    // Numeric gears (1-9) are manual mode
    bool ok = false;
    gear.toInt(&ok);
    if (ok) return DriveMode::Manual;

    return DriveMode::Drive;  // Default
}

} // namespace driver
} // namespace automotive
