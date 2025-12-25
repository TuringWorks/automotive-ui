// PermissionManager.h
// Permission management for privileged operations
// Security: CR-INF-002 - Privileged actions require explicit permission

#ifndef AUTOMOTIVE_PERMISSION_MANAGER_H
#define AUTOMOTIVE_PERMISSION_MANAGER_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>

namespace automotive {
namespace infotainment {

/**
 * @brief Permission types
 */
enum class Permission : uint32_t {
    None = 0,

    // Media permissions
    MediaPlayback = 1 << 0,
    MediaLibraryRead = 1 << 1,
    MediaLibraryWrite = 1 << 2,

    // Phone permissions
    PhoneContacts = 1 << 3,
    PhoneCalls = 1 << 4,
    PhoneMessages = 1 << 5,

    // Navigation permissions
    NavigationLocation = 1 << 6,
    NavigationRouting = 1 << 7,

    // Vehicle permissions (privileged - CR-INF-002)
    VehicleSettingsRead = 1 << 8,
    VehicleSettingsWrite = 1 << 9,      // Requires explicit consent
    VehicleDriveModes = 1 << 10,        // Privileged
    VehicleRemoteControl = 1 << 11,     // Highly privileged

    // System permissions
    SystemSettings = 1 << 12,
    SystemDebug = 1 << 13,

    All = 0xFFFFFFFF
};

Q_DECLARE_FLAGS(Permissions, Permission)
Q_DECLARE_OPERATORS_FOR_FLAGS(Permissions)

/**
 * @brief Permission request result
 */
struct PermissionResult {
    bool granted{false};
    QString reason;
    qint64 timestamp{0};
};

/**
 * @brief Permission manager for infotainment
 *
 * Security: CR-INF-002 - Enforces permission model for sensitive operations
 *
 * Features:
 * - Permission gating for privileged actions
 * - Audit logging of permission requests
 * - User consent flows (scaffold)
 */
class PermissionManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(QStringList grantedPermissions READ grantedPermissionsList NOTIFY permissionsChanged)

public:
    explicit PermissionManager(QObject* parent = nullptr);
    ~PermissionManager() override;

    /**
     * @brief Check if permission is granted
     * @param permission Permission to check
     * @return true if granted
     */
    bool hasPermission(Permission permission) const;

    /**
     * @brief Check if permission is granted (Q_INVOKABLE)
     * @param permissionName Permission name string
     */
    Q_INVOKABLE bool checkPermission(const QString& permissionName) const;

    /**
     * @brief Request permission
     * @param permission Permission to request
     * @param reason Reason for request
     * @return Result of the request
     *
     * Security: Logs audit event (CR-INF-002)
     */
    PermissionResult requestPermission(Permission permission, const QString& reason);

    /**
     * @brief Request permission (Q_INVOKABLE)
     */
    Q_INVOKABLE bool requestPermission(const QString& permissionName, const QString& reason);

    /**
     * @brief Grant permission (internal use)
     */
    void grantPermission(Permission permission);

    /**
     * @brief Revoke permission
     */
    void revokePermission(Permission permission);

    /**
     * @brief Get list of granted permissions
     */
    Permissions grantedPermissions() const { return m_granted; }
    QStringList grantedPermissionsList() const;

    /**
     * @brief Get all available permissions
     */
    Q_INVOKABLE QVariantList availablePermissions() const;

    /**
     * @brief Check if permission is privileged (requires extra consent)
     */
    static bool isPrivileged(Permission permission);

    /**
     * @brief Convert permission to string
     */
    static QString permissionToString(Permission permission);

    /**
     * @brief Convert string to permission
     */
    static Permission stringToPermission(const QString& name);

signals:
    void permissionsChanged();
    void permissionGranted(const QString& permission);
    void permissionRevoked(const QString& permission);
    void permissionDenied(const QString& permission, const QString& reason);

private:
    void logAuditEvent(const QString& action, Permission permission, bool granted);

    Permissions m_granted{Permission::None};
};

} // namespace infotainment
} // namespace automotive

Q_DECLARE_METATYPE(automotive::infotainment::Permission)

#endif // AUTOMOTIVE_PERMISSION_MANAGER_H
