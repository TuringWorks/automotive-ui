// ProjectionController.h
// Smartphone projection (Apple CarPlay / Android Auto) controller (mock)

#ifndef AUTOMOTIVE_PROJECTION_CONTROLLER_H
#define AUTOMOTIVE_PROJECTION_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace automotive {
namespace infotainment {

class ProjectionController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool carPlayAvailable READ carPlayAvailable NOTIFY carPlayAvailableChanged)
    Q_PROPERTY(bool carPlayConnected READ carPlayConnected NOTIFY carPlayConnectionChanged)
    Q_PROPERTY(bool carPlayWirelessEnabled READ carPlayWirelessEnabled WRITE setCarPlayWirelessEnabled NOTIFY carPlayWirelessEnabledChanged)
    Q_PROPERTY(QString carPlayDeviceName READ carPlayDeviceName NOTIFY carPlayDeviceNameChanged)
    Q_PROPERTY(QString carPlayStatus READ carPlayStatus NOTIFY carPlayStatusChanged)

    Q_PROPERTY(bool androidAutoAvailable READ androidAutoAvailable NOTIFY androidAutoAvailableChanged)
    Q_PROPERTY(bool androidAutoConnected READ androidAutoConnected NOTIFY androidAutoConnectionChanged)
    Q_PROPERTY(bool androidAutoWirelessEnabled READ androidAutoWirelessEnabled WRITE setAndroidAutoWirelessEnabled NOTIFY androidAutoWirelessEnabledChanged)
    Q_PROPERTY(QString androidAutoDeviceName READ androidAutoDeviceName NOTIFY androidAutoDeviceNameChanged)
    Q_PROPERTY(QString androidAutoStatus READ androidAutoStatus NOTIFY androidAutoStatusChanged)

    Q_PROPERTY(QString activeProjection READ activeProjection NOTIFY activeProjectionChanged)
    Q_PROPERTY(QString statusBanner READ statusBanner NOTIFY statusBannerChanged)
    Q_PROPERTY(bool sessionActive READ sessionActive NOTIFY sessionStateChanged)
    Q_PROPERTY(QString sessionPlatform READ sessionPlatform NOTIFY sessionStateChanged)
    Q_PROPERTY(QString sessionSummary READ sessionSummary NOTIFY sessionStateChanged)
    Q_PROPERTY(QVariantList sessionApps READ sessionApps NOTIFY sessionAppsChanged)
    Q_PROPERTY(QVariantMap sessionNavTile READ sessionNavTile NOTIFY sessionStateChanged)
    Q_PROPERTY(QVariantMap sessionMediaTile READ sessionMediaTile NOTIFY sessionStateChanged)

public:
    explicit ProjectionController(QObject* parent = nullptr);
    ~ProjectionController() override;

    bool carPlayAvailable() const { return m_carPlayAvailable; }
    bool carPlayConnected() const { return m_carPlayConnected; }
    bool carPlayWirelessEnabled() const { return m_carPlayWirelessEnabled; }
    QString carPlayDeviceName() const { return m_carPlayDeviceName; }
    QString carPlayStatus() const { return m_carPlayStatus; }

    bool androidAutoAvailable() const { return m_androidAutoAvailable; }
    bool androidAutoConnected() const { return m_androidAutoConnected; }
    bool androidAutoWirelessEnabled() const { return m_androidAutoWirelessEnabled; }
    QString androidAutoDeviceName() const { return m_androidAutoDeviceName; }
    QString androidAutoStatus() const { return m_androidAutoStatus; }

    QString activeProjection() const { return m_activeProjection; }
    QString statusBanner() const { return m_statusBanner; }
    bool sessionActive() const { return m_sessionActive; }
    QString sessionPlatform() const { return m_sessionPlatform; }
    QString sessionSummary() const { return m_sessionSummary; }
    QVariantList sessionApps() const { return m_sessionApps; }
    QVariantMap sessionNavTile() const { return m_sessionNavTile; }
    QVariantMap sessionMediaTile() const { return m_sessionMediaTile; }

public slots:
    void connectCarPlay();
    void disconnectCarPlay();
    void connectAndroidAuto();
    void disconnectAndroidAuto();
    void setCarPlayWirelessEnabled(bool enabled);
    void setAndroidAutoWirelessEnabled(bool enabled);

signals:
    void carPlayAvailableChanged(bool available);
    void carPlayConnectionChanged(bool connected);
    void carPlayWirelessEnabledChanged(bool enabled);
    void carPlayDeviceNameChanged(const QString& name);
    void carPlayStatusChanged(const QString& status);

    void androidAutoAvailableChanged(bool available);
    void androidAutoConnectionChanged(bool connected);
    void androidAutoWirelessEnabledChanged(bool enabled);
    void androidAutoDeviceNameChanged(const QString& name);
    void androidAutoStatusChanged(const QString& status);

    void activeProjectionChanged(const QString& projection);
    void statusBannerChanged(const QString& banner);
    void sessionStateChanged();
    void sessionAppsChanged();

private:
    void setActiveProjection(const QString& projection);
    void updateStatusStrings();
    void updateSessionMock();

    bool m_carPlayAvailable{true};
    bool m_carPlayConnected{false};
    bool m_carPlayWirelessEnabled{true};
    QString m_carPlayDeviceName{QStringLiteral("Ava's iPhone")};
    QString m_carPlayStatus;

    bool m_androidAutoAvailable{true};
    bool m_androidAutoConnected{false};
    bool m_androidAutoWirelessEnabled{false};
    QString m_androidAutoDeviceName{QStringLiteral("Pixel 9 Pro")};
    QString m_androidAutoStatus;

    QString m_activeProjection{QStringLiteral("None")};
    QString m_statusBanner;

    bool m_sessionActive{false};
    QString m_sessionPlatform{QStringLiteral("None")};
    QString m_sessionSummary{QStringLiteral("Connect a trusted device to launch projection")};
    QVariantList m_sessionApps;
    QVariantMap m_sessionNavTile;
    QVariantMap m_sessionMediaTile;
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_PROJECTION_CONTROLLER_H
