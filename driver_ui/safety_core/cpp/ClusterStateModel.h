// ClusterStateModel.h
// Central state model for cluster display
// Safety: This is the primary safety-relevant data model
// All signal processing, validation, and formatting happens here (not in QML)

#ifndef AUTOMOTIVE_CLUSTER_STATE_MODEL_H
#define AUTOMOTIVE_CLUSTER_STATE_MODEL_H

#include <QObject>
#include <QVariant>
#include "signal/SignalHub.h"
#include "sched/DeterministicScheduler.h"

namespace automotive {
namespace driver {

/**
 * @brief Drive mode enumeration
 */
enum class DriveMode : uint8_t {
    Park = 0,
    Reverse,
    Neutral,
    Drive,
    Sport,
    Eco,
    Manual
};

/**
 * @brief Cluster operating state
 */
enum class ClusterState : uint8_t {
    Normal = 0,     ///< All systems nominal
    Warning,        ///< Non-critical warnings present
    Degraded,       ///< Critical signal loss - reduced display (SR-CL-004)
    Fault           ///< System fault - minimal display
};

/**
 * @brief Cluster state model - Central safety-critical data model
 *
 * This class is the single source of truth for all cluster display data.
 * Safety requirements:
 * - SR-CL-001: Speed display shall be updated at ≥10Hz
 * - SR-CL-002: Invalid signal ranges shall be clamped and flagged
 * - SR-CL-003: Critical telltales cannot be occluded
 * - SR-CL-004: Degraded mode on loss of signal integrity
 *
 * Design:
 * - All signal processing happens in C++, not QML
 * - Bounded memory allocations after initialization
 * - Deterministic update cycle (20Hz signal, 60Hz render)
 * - Thread-safe property access
 */
class ClusterStateModel : public QObject {
    Q_OBJECT

    // Speed (SR-CL-001)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(bool speedValid READ speedValid NOTIFY speedValidChanged)
    Q_PROPERTY(bool speedStale READ speedStale NOTIFY speedStaleChanged)
    Q_PROPERTY(QString speedUnit READ speedUnit NOTIFY speedUnitChanged)
    Q_PROPERTY(QString speedDisplay READ speedDisplay NOTIFY speedDisplayChanged)

    // Gear
    Q_PROPERTY(QString gear READ gear NOTIFY gearChanged)
    Q_PROPERTY(bool gearValid READ gearValid NOTIFY gearValidChanged)
    Q_PROPERTY(DriveMode driveMode READ driveMode NOTIFY driveModeChanged)

    // Energy
    Q_PROPERTY(double batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool batteryValid READ batteryValid NOTIFY batteryValidChanged)
    Q_PROPERTY(double range READ range NOTIFY rangeChanged)
    Q_PROPERTY(bool rangeValid READ rangeValid NOTIFY rangeValidChanged)
    Q_PROPERTY(double powerConsumption READ powerConsumption NOTIFY powerConsumptionChanged)

    // State
    Q_PROPERTY(ClusterState clusterState READ clusterState NOTIFY clusterStateChanged)
    Q_PROPERTY(bool isDegraded READ isDegraded NOTIFY isDegradedChanged)
    Q_PROPERTY(int invalidSignalCount READ invalidSignalCount NOTIFY invalidSignalCountChanged)

    // Environment
    Q_PROPERTY(double outsideTemp READ outsideTemp NOTIFY outsideTempChanged)
    Q_PROPERTY(QString timeDisplay READ timeDisplay NOTIFY timeDisplayChanged)

public:
    explicit ClusterStateModel(signal::SignalHub* signalHub,
                               QObject* parent = nullptr);
    ~ClusterStateModel() override;

    // Prevent copying (safety: single instance per cluster)
    ClusterStateModel(const ClusterStateModel&) = delete;
    ClusterStateModel& operator=(const ClusterStateModel&) = delete;

    // Speed properties
    double speed() const { return m_speed; }
    bool speedValid() const { return m_speedValid; }
    bool speedStale() const { return m_speedStale; }
    QString speedUnit() const { return m_speedUnit; }
    QString speedDisplay() const;  // Formatted for display

    // Gear properties
    QString gear() const { return m_gear; }
    bool gearValid() const { return m_gearValid; }
    DriveMode driveMode() const { return m_driveMode; }

    // Energy properties
    double batteryLevel() const { return m_batteryLevel; }
    bool batteryValid() const { return m_batteryValid; }
    double range() const { return m_range; }
    bool rangeValid() const { return m_rangeValid; }
    double powerConsumption() const { return m_powerConsumption; }

    // State
    ClusterState clusterState() const { return m_clusterState; }
    bool isDegraded() const { return m_clusterState == ClusterState::Degraded ||
                                     m_clusterState == ClusterState::Fault; }
    int invalidSignalCount() const { return m_invalidSignalCount; }

    // Environment
    double outsideTemp() const { return m_outsideTemp; }
    QString timeDisplay() const { return m_timeDisplay; }

    /**
     * @brief Process tick update (called from scheduler)
     * @param tickNumber Current tick number
     * @param elapsedMs Elapsed time since start
     */
    void processTick(uint64_t tickNumber, qint64 elapsedMs);

    /**
     * @brief Force degraded mode (for testing)
     */
    void forceDegradedMode(bool degraded);

signals:
    // Speed signals
    void speedChanged(double speed);
    void speedValidChanged(bool valid);
    void speedStaleChanged(bool stale);
    void speedUnitChanged(const QString& unit);
    void speedDisplayChanged(const QString& display);

    // Gear signals
    void gearChanged(const QString& gear);
    void gearValidChanged(bool valid);
    void driveModeChanged(DriveMode mode);

    // Energy signals
    void batteryLevelChanged(double level);
    void batteryValidChanged(bool valid);
    void rangeChanged(double range);
    void rangeValidChanged(bool valid);
    void powerConsumptionChanged(double power);

    // State signals
    void clusterStateChanged(ClusterState state);
    void isDegradedChanged(bool degraded);
    void invalidSignalCountChanged(int count);

    // Environment signals
    void outsideTempChanged(double temp);
    void timeDisplayChanged(const QString& time);

private slots:
    void onSignalUpdated(const QString& signalId, const signal::SignalValue& value);
    void onDegradedModeChanged(bool degraded);

private:
    void updateClusterState();
    DriveMode gearToDriveMode(const QString& gear) const;

    signal::SignalHub* m_signalHub{nullptr};

    // Speed state
    double m_speed{0.0};
    bool m_speedValid{false};
    bool m_speedStale{false};
    QString m_speedUnit{QStringLiteral("km/h")};
    bool m_useKmh{true};

    // Gear state
    QString m_gear{QStringLiteral("P")};
    bool m_gearValid{false};
    DriveMode m_driveMode{DriveMode::Park};

    // Energy state
    double m_batteryLevel{0.0};
    bool m_batteryValid{false};
    double m_range{0.0};
    bool m_rangeValid{false};
    double m_powerConsumption{0.0};

    // Cluster state
    ClusterState m_clusterState{ClusterState::Normal};
    int m_invalidSignalCount{0};
    bool m_forcedDegraded{false};

    // Environment
    double m_outsideTemp{0.0};
    QString m_timeDisplay;

    // Tick tracking
    uint64_t m_lastSpeedUpdateTick{0};
};

} // namespace driver
} // namespace automotive

Q_DECLARE_METATYPE(automotive::driver::DriveMode)
Q_DECLARE_METATYPE(automotive::driver::ClusterState)

#endif // AUTOMOTIVE_CLUSTER_STATE_MODEL_H
