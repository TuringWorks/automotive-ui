// VehicleSettingsController.h
// Vehicle settings controller with permission gating
// Security: CR-INF-002 - Privileged actions require permission

#ifndef AUTOMOTIVE_VEHICLE_SETTINGS_CONTROLLER_H
#define AUTOMOTIVE_VEHICLE_SETTINGS_CONTROLLER_H

#include <QObject>
#include <QString>
#include "PermissionManager.h"

namespace automotive {
namespace infotainment {

/**
 * @brief Vehicle settings controller
 *
 * Security: CR-INF-002 - Privileged vehicle settings require permission
 */
class VehicleSettingsController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString driveMode READ driveMode NOTIFY driveModeChanged)
    Q_PROPERTY(int hvacTempDriver READ hvacTempDriver NOTIFY hvacChanged)
    Q_PROPERTY(int hvacTempPassenger READ hvacTempPassenger NOTIFY hvacChanged)
    Q_PROPERTY(int hvacFanSpeed READ hvacFanSpeed NOTIFY hvacChanged)
    Q_PROPERTY(bool hvacAcOn READ hvacAcOn NOTIFY hvacChanged)
    Q_PROPERTY(bool lightsAuto READ lightsAuto NOTIFY lightingChanged)
    Q_PROPERTY(int ambientBrightness READ ambientBrightness NOTIFY lightingChanged)
    Q_PROPERTY(bool canModifyDriveMode READ canModifyDriveMode NOTIFY permissionsChanged)

    // Extended climate controls
    Q_PROPERTY(int seatHeatDriver READ seatHeatDriver NOTIFY hvacChanged)
    Q_PROPERTY(int seatHeatPassenger READ seatHeatPassenger NOTIFY hvacChanged)
    Q_PROPERTY(QString fanDirection READ fanDirection NOTIFY hvacChanged)
    Q_PROPERTY(bool defrostFront READ defrostFront NOTIFY hvacChanged)
    Q_PROPERTY(bool defrostRear READ defrostRear NOTIFY hvacChanged)
    Q_PROPERTY(bool hvacSync READ hvacSync NOTIFY hvacChanged)
    Q_PROPERTY(bool hvacRecirculation READ hvacRecirculation NOTIFY hvacChanged)
    Q_PROPERTY(bool hvacAutoMode READ hvacAutoMode NOTIFY hvacChanged)

public:
    explicit VehicleSettingsController(PermissionManager* permissionManager,
                                       QObject* parent = nullptr);
    ~VehicleSettingsController() override;

    QString driveMode() const { return m_driveMode; }
    int hvacTempDriver() const { return m_hvacTempDriver; }
    int hvacTempPassenger() const { return m_hvacTempPassenger; }
    int hvacFanSpeed() const { return m_hvacFanSpeed; }
    bool hvacAcOn() const { return m_hvacAcOn; }
    bool lightsAuto() const { return m_lightsAuto; }
    int ambientBrightness() const { return m_ambientBrightness; }

    // Extended climate getters
    int seatHeatDriver() const { return m_seatHeatDriver; }
    int seatHeatPassenger() const { return m_seatHeatPassenger; }
    QString fanDirection() const { return m_fanDirection; }
    bool defrostFront() const { return m_defrostFront; }
    bool defrostRear() const { return m_defrostRear; }
    bool hvacSync() const { return m_hvacSync; }
    bool hvacRecirculation() const { return m_hvacRecirculation; }
    bool hvacAutoMode() const { return m_hvacAutoMode; }

    bool canModifyDriveMode() const;

public slots:
    /**
     * @brief Set drive mode (privileged - requires VehicleDriveModes permission)
     * @param mode Drive mode name
     * @return true if successful
     */
    bool setDriveMode(const QString& mode);

    /**
     * @brief Set HVAC driver temperature
     */
    void setHvacTempDriver(int temp);

    /**
     * @brief Set HVAC passenger temperature
     */
    void setHvacTempPassenger(int temp);

    /**
     * @brief Set HVAC fan speed
     */
    void setHvacFanSpeed(int speed);

    /**
     * @brief Set HVAC AC on/off
     */
    void setHvacAcOn(bool on);

    /**
     * @brief Set lights auto mode
     */
    void setLightsAuto(bool autoMode);

    /**
     * @brief Set ambient lighting brightness
     */
    void setAmbientBrightness(int brightness);

    /**
     * @brief Request drive mode permission
     */
    bool requestDriveModePermission();

    // Extended climate setters
    void setSeatHeatDriver(int level);
    void setSeatHeatPassenger(int level);
    void setFanDirection(const QString& direction);
    void setDefrostFront(bool on);
    void setDefrostRear(bool on);
    void setHvacSync(bool on);
    void setHvacRecirculation(bool on);
    void setHvacAutoMode(bool on);

signals:
    void driveModeChanged(const QString& mode);
    void hvacChanged();
    void lightingChanged();
    void permissionsChanged();
    void permissionDenied(const QString& action);

private:
    PermissionManager* m_permissionManager{nullptr};

    QString m_driveMode{QStringLiteral("Normal")};
    int m_hvacTempDriver{22};
    int m_hvacTempPassenger{22};
    int m_hvacFanSpeed{3};
    bool m_hvacAcOn{true};
    bool m_lightsAuto{true};
    int m_ambientBrightness{50};

    // Extended climate state
    int m_seatHeatDriver{0};      // 0=off, 1-3=heat levels
    int m_seatHeatPassenger{0};
    QString m_fanDirection{QStringLiteral("face")};  // face, feet, both, windshield
    bool m_defrostFront{false};
    bool m_defrostRear{false};
    bool m_hvacSync{false};
    bool m_hvacRecirculation{false};
    bool m_hvacAutoMode{false};
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_VEHICLE_SETTINGS_CONTROLLER_H
