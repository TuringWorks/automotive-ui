// VehicleSettingsController.cpp
// Vehicle settings controller implementation

#include "VehicleSettingsController.h"
#include "logging/Logger.h"

namespace automotive {
namespace infotainment {

VehicleSettingsController::VehicleSettingsController(PermissionManager* permissionManager,
                                                     QObject* parent)
    : QObject(parent)
    , m_permissionManager(permissionManager)
{
    if (m_permissionManager) {
        connect(m_permissionManager, &PermissionManager::permissionsChanged,
                this, &VehicleSettingsController::permissionsChanged);
    }
}

VehicleSettingsController::~VehicleSettingsController() = default;

bool VehicleSettingsController::canModifyDriveMode() const
{
    return m_permissionManager &&
           m_permissionManager->hasPermission(Permission::VehicleDriveModes);
}

bool VehicleSettingsController::setDriveMode(const QString& mode)
{
    // Security: CR-INF-002 - Check permission before modifying
    if (!m_permissionManager ||
        !m_permissionManager->hasPermission(Permission::VehicleDriveModes)) {

        AUTO_LOG_WARNING("security",
            QString::fromLatin1("Drive mode change denied - missing permission"));

        emit permissionDenied(QStringLiteral("setDriveMode"));
        return false;
    }

    if (m_driveMode != mode) {
        m_driveMode = mode;

        // Audit log for privileged action
        logging::Logger::instance().audit(
            QStringLiteral("setDriveMode"),
            QStringLiteral("driveMode"),
            QStringLiteral("success"),
            {{QStringLiteral("newMode"), mode}}
        );

        emit driveModeChanged(m_driveMode);
    }

    return true;
}

void VehicleSettingsController::setHvacTempDriver(int temp)
{
    temp = qBound(16, temp, 30);
    if (m_hvacTempDriver != temp) {
        m_hvacTempDriver = temp;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacTempPassenger(int temp)
{
    temp = qBound(16, temp, 30);
    if (m_hvacTempPassenger != temp) {
        m_hvacTempPassenger = temp;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacFanSpeed(int speed)
{
    speed = qBound(0, speed, 7);
    if (m_hvacFanSpeed != speed) {
        m_hvacFanSpeed = speed;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacAcOn(bool on)
{
    if (m_hvacAcOn != on) {
        m_hvacAcOn = on;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setLightsAuto(bool autoMode)
{
    if (m_lightsAuto != autoMode) {
        m_lightsAuto = autoMode;
        emit lightingChanged();
    }
}

void VehicleSettingsController::setAmbientBrightness(int brightness)
{
    brightness = qBound(0, brightness, 100);
    if (m_ambientBrightness != brightness) {
        m_ambientBrightness = brightness;
        emit lightingChanged();
    }
}

bool VehicleSettingsController::requestDriveModePermission()
{
    if (!m_permissionManager) {
        return false;
    }

    auto result = m_permissionManager->requestPermission(
        Permission::VehicleDriveModes,
        QStringLiteral("Change vehicle drive mode")
    );

    return result.granted;
}

void VehicleSettingsController::setSeatHeatDriver(int level)
{
    level = qBound(0, level, 3);
    if (m_seatHeatDriver != level) {
        m_seatHeatDriver = level;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setSeatHeatPassenger(int level)
{
    level = qBound(0, level, 3);
    if (m_seatHeatPassenger != level) {
        m_seatHeatPassenger = level;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setFanDirection(const QString& direction)
{
    if (m_fanDirection != direction) {
        m_fanDirection = direction;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setDefrostFront(bool on)
{
    if (m_defrostFront != on) {
        m_defrostFront = on;
        // Front defrost typically sets fan to windshield
        if (on) {
            m_fanDirection = QStringLiteral("windshield");
        }
        emit hvacChanged();
    }
}

void VehicleSettingsController::setDefrostRear(bool on)
{
    if (m_defrostRear != on) {
        m_defrostRear = on;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacSync(bool on)
{
    if (m_hvacSync != on) {
        m_hvacSync = on;
        // Sync passenger temp to driver temp
        if (on && m_hvacTempPassenger != m_hvacTempDriver) {
            m_hvacTempPassenger = m_hvacTempDriver;
        }
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacRecirculation(bool on)
{
    if (m_hvacRecirculation != on) {
        m_hvacRecirculation = on;
        emit hvacChanged();
    }
}

void VehicleSettingsController::setHvacAutoMode(bool on)
{
    if (m_hvacAutoMode != on) {
        m_hvacAutoMode = on;
        emit hvacChanged();
    }
}

} // namespace infotainment
} // namespace automotive
