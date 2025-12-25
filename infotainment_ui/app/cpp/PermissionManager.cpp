// PermissionManager.cpp
// Permission manager implementation

#include "PermissionManager.h"
#include "logging/Logger.h"
#include <QDateTime>

namespace automotive {
namespace infotainment {

PermissionManager::PermissionManager(QObject* parent)
    : QObject(parent)
{
    // Grant default non-privileged permissions
    m_granted = Permission::MediaPlayback |
                Permission::MediaLibraryRead |
                Permission::NavigationLocation |
                Permission::NavigationRouting |
                Permission::VehicleSettingsRead;
}

PermissionManager::~PermissionManager() = default;

bool PermissionManager::hasPermission(Permission permission) const
{
    return m_granted.testFlag(permission);
}

bool PermissionManager::checkPermission(const QString& permissionName) const
{
    return hasPermission(stringToPermission(permissionName));
}

PermissionResult PermissionManager::requestPermission(Permission permission,
                                                       const QString& reason)
{
    PermissionResult result;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();

    // Already granted
    if (hasPermission(permission)) {
        result.granted = true;
        result.reason = QStringLiteral("Already granted");
        return result;
    }

    // Check if privileged (CR-INF-002)
    if (isPrivileged(permission)) {
        // For privileged permissions, we need explicit user consent
        // In a real implementation, this would show a consent dialog
        // For now, we deny and log

        result.granted = false;
        result.reason = QStringLiteral("Privileged permission requires user consent");

        logAuditEvent(QStringLiteral("request_denied"), permission, false);
        emit permissionDenied(permissionToString(permission), result.reason);

        AUTO_LOG_WARNING("security",
            QString::fromLatin1("Privileged permission denied: %1 - %2")
                .arg(permissionToString(permission), reason));

        return result;
    }

    // Grant non-privileged permissions
    grantPermission(permission);
    result.granted = true;
    result.reason = QStringLiteral("Granted");

    logAuditEvent(QStringLiteral("request_granted"), permission, true);

    return result;
}

bool PermissionManager::requestPermission(const QString& permissionName,
                                           const QString& reason)
{
    auto result = requestPermission(stringToPermission(permissionName), reason);
    return result.granted;
}

void PermissionManager::grantPermission(Permission permission)
{
    if (!m_granted.testFlag(permission)) {
        m_granted |= permission;
        emit permissionsChanged();
        emit permissionGranted(permissionToString(permission));

        logAuditEvent(QStringLiteral("granted"), permission, true);
    }
}

void PermissionManager::revokePermission(Permission permission)
{
    if (m_granted.testFlag(permission)) {
        m_granted.setFlag(permission, false);
        emit permissionsChanged();
        emit permissionRevoked(permissionToString(permission));

        logAuditEvent(QStringLiteral("revoked"), permission, false);
    }
}

QStringList PermissionManager::grantedPermissionsList() const
{
    QStringList list;

    if (m_granted.testFlag(Permission::MediaPlayback))
        list << QStringLiteral("MediaPlayback");
    if (m_granted.testFlag(Permission::MediaLibraryRead))
        list << QStringLiteral("MediaLibraryRead");
    if (m_granted.testFlag(Permission::MediaLibraryWrite))
        list << QStringLiteral("MediaLibraryWrite");
    if (m_granted.testFlag(Permission::PhoneContacts))
        list << QStringLiteral("PhoneContacts");
    if (m_granted.testFlag(Permission::PhoneCalls))
        list << QStringLiteral("PhoneCalls");
    if (m_granted.testFlag(Permission::NavigationLocation))
        list << QStringLiteral("NavigationLocation");
    if (m_granted.testFlag(Permission::VehicleSettingsRead))
        list << QStringLiteral("VehicleSettingsRead");
    if (m_granted.testFlag(Permission::VehicleSettingsWrite))
        list << QStringLiteral("VehicleSettingsWrite");
    if (m_granted.testFlag(Permission::VehicleDriveModes))
        list << QStringLiteral("VehicleDriveModes");

    return list;
}

QVariantList PermissionManager::availablePermissions() const
{
    QVariantList list;

    auto addPermission = [&](Permission p, const QString& name,
                             const QString& desc, bool privileged) {
        QVariantMap map;
        map[QStringLiteral("id")] = static_cast<int>(p);
        map[QStringLiteral("name")] = name;
        map[QStringLiteral("description")] = desc;
        map[QStringLiteral("privileged")] = privileged;
        map[QStringLiteral("granted")] = hasPermission(p);
        list.append(map);
    };

    addPermission(Permission::MediaPlayback, QStringLiteral("MediaPlayback"),
                  QStringLiteral("Play media content"), false);
    addPermission(Permission::MediaLibraryRead, QStringLiteral("MediaLibraryRead"),
                  QStringLiteral("Read media library"), false);
    addPermission(Permission::PhoneContacts, QStringLiteral("PhoneContacts"),
                  QStringLiteral("Access phone contacts"), false);
    addPermission(Permission::PhoneCalls, QStringLiteral("PhoneCalls"),
                  QStringLiteral("Make and receive calls"), false);
    addPermission(Permission::NavigationLocation, QStringLiteral("NavigationLocation"),
                  QStringLiteral("Access location for navigation"), false);
    addPermission(Permission::VehicleSettingsRead, QStringLiteral("VehicleSettingsRead"),
                  QStringLiteral("Read vehicle settings"), false);
    addPermission(Permission::VehicleSettingsWrite, QStringLiteral("VehicleSettingsWrite"),
                  QStringLiteral("Modify vehicle settings"), true);
    addPermission(Permission::VehicleDriveModes, QStringLiteral("VehicleDriveModes"),
                  QStringLiteral("Change drive modes"), true);
    addPermission(Permission::VehicleRemoteControl, QStringLiteral("VehicleRemoteControl"),
                  QStringLiteral("Remote vehicle control"), true);

    return list;
}

bool PermissionManager::isPrivileged(Permission permission)
{
    switch (permission) {
    case Permission::VehicleSettingsWrite:
    case Permission::VehicleDriveModes:
    case Permission::VehicleRemoteControl:
    case Permission::SystemDebug:
        return true;
    default:
        return false;
    }
}

QString PermissionManager::permissionToString(Permission permission)
{
    switch (permission) {
    case Permission::MediaPlayback: return QStringLiteral("MediaPlayback");
    case Permission::MediaLibraryRead: return QStringLiteral("MediaLibraryRead");
    case Permission::MediaLibraryWrite: return QStringLiteral("MediaLibraryWrite");
    case Permission::PhoneContacts: return QStringLiteral("PhoneContacts");
    case Permission::PhoneCalls: return QStringLiteral("PhoneCalls");
    case Permission::PhoneMessages: return QStringLiteral("PhoneMessages");
    case Permission::NavigationLocation: return QStringLiteral("NavigationLocation");
    case Permission::NavigationRouting: return QStringLiteral("NavigationRouting");
    case Permission::VehicleSettingsRead: return QStringLiteral("VehicleSettingsRead");
    case Permission::VehicleSettingsWrite: return QStringLiteral("VehicleSettingsWrite");
    case Permission::VehicleDriveModes: return QStringLiteral("VehicleDriveModes");
    case Permission::VehicleRemoteControl: return QStringLiteral("VehicleRemoteControl");
    case Permission::SystemSettings: return QStringLiteral("SystemSettings");
    case Permission::SystemDebug: return QStringLiteral("SystemDebug");
    default: return QStringLiteral("Unknown");
    }
}

Permission PermissionManager::stringToPermission(const QString& name)
{
    if (name == QLatin1String("MediaPlayback")) return Permission::MediaPlayback;
    if (name == QLatin1String("MediaLibraryRead")) return Permission::MediaLibraryRead;
    if (name == QLatin1String("MediaLibraryWrite")) return Permission::MediaLibraryWrite;
    if (name == QLatin1String("PhoneContacts")) return Permission::PhoneContacts;
    if (name == QLatin1String("PhoneCalls")) return Permission::PhoneCalls;
    if (name == QLatin1String("NavigationLocation")) return Permission::NavigationLocation;
    if (name == QLatin1String("VehicleSettingsRead")) return Permission::VehicleSettingsRead;
    if (name == QLatin1String("VehicleSettingsWrite")) return Permission::VehicleSettingsWrite;
    if (name == QLatin1String("VehicleDriveModes")) return Permission::VehicleDriveModes;
    if (name == QLatin1String("VehicleRemoteControl")) return Permission::VehicleRemoteControl;
    return Permission::None;
}

void PermissionManager::logAuditEvent(const QString& action,
                                       Permission permission,
                                       bool granted)
{
    logging::Logger::instance().audit(
        action,
        permissionToString(permission),
        granted ? QStringLiteral("granted") : QStringLiteral("denied")
    );
}

} // namespace infotainment
} // namespace automotive
