// ProjectionController.cpp
// Smartphone projection controller implementation

#include "ProjectionController.h"

#include <QVariantMap>

namespace automotive {
namespace infotainment {

ProjectionController::ProjectionController(QObject* parent)
    : QObject(parent)
{
    setActiveProjection(QStringLiteral("None"));
    updateStatusStrings();
}

ProjectionController::~ProjectionController() = default;

void ProjectionController::connectCarPlay()
{
    if (!m_carPlayAvailable)
        return;

    bool changed = false;
    if (!m_carPlayConnected) {
        m_carPlayConnected = true;
        changed = true;
        emit carPlayConnectionChanged(true);
    }

    if (m_androidAutoConnected) {
        m_androidAutoConnected = false;
        emit androidAutoConnectionChanged(false);
    }

    if (changed || m_activeProjection != QStringLiteral("Apple CarPlay"))
        setActiveProjection(QStringLiteral("Apple CarPlay"));

    updateStatusStrings();
}

void ProjectionController::disconnectCarPlay()
{
    if (!m_carPlayConnected)
        return;

    m_carPlayConnected = false;
    emit carPlayConnectionChanged(false);

    if (!m_androidAutoConnected)
        setActiveProjection(QStringLiteral("None"));

    updateStatusStrings();
}

void ProjectionController::connectAndroidAuto()
{
    if (!m_androidAutoAvailable)
        return;

    bool changed = false;
    if (!m_androidAutoConnected) {
        m_androidAutoConnected = true;
        changed = true;
        emit androidAutoConnectionChanged(true);
    }

    if (m_carPlayConnected) {
        m_carPlayConnected = false;
        emit carPlayConnectionChanged(false);
    }

    if (changed || m_activeProjection != QStringLiteral("Android Auto"))
        setActiveProjection(QStringLiteral("Android Auto"));

    updateStatusStrings();
}

void ProjectionController::disconnectAndroidAuto()
{
    if (!m_androidAutoConnected)
        return;

    m_androidAutoConnected = false;
    emit androidAutoConnectionChanged(false);

    if (!m_carPlayConnected)
        setActiveProjection(QStringLiteral("None"));

    updateStatusStrings();
}

void ProjectionController::setCarPlayWirelessEnabled(bool enabled)
{
    if (m_carPlayWirelessEnabled == enabled)
        return;

    m_carPlayWirelessEnabled = enabled;
    emit carPlayWirelessEnabledChanged(enabled);
    updateStatusStrings();
}

void ProjectionController::setAndroidAutoWirelessEnabled(bool enabled)
{
    if (m_androidAutoWirelessEnabled == enabled)
        return;

    m_androidAutoWirelessEnabled = enabled;
    emit androidAutoWirelessEnabledChanged(enabled);
    updateStatusStrings();
}

void ProjectionController::setActiveProjection(const QString& projection)
{
    bool projectionChanged = m_activeProjection != projection;
    if (projectionChanged) {
        m_activeProjection = projection;
        emit activeProjectionChanged(m_activeProjection);
    }

    QString newBanner;
    if (projection == QStringLiteral("Apple CarPlay")) {
        newBanner = QStringLiteral("Apple CarPlay session is active");
    } else if (projection == QStringLiteral("Android Auto")) {
        newBanner = QStringLiteral("Android Auto session is active");
    } else {
        newBanner = QStringLiteral("No projection session active");
    }

    if (m_statusBanner != newBanner) {
        m_statusBanner = newBanner;
        emit statusBannerChanged(m_statusBanner);
    }

    updateSessionMock();
}

void ProjectionController::updateStatusStrings()
{
    QString newCarPlayStatus;
    if (!m_carPlayAvailable) {
        newCarPlayStatus = QStringLiteral("Unavailable");
    } else if (m_carPlayConnected) {
        newCarPlayStatus = QStringLiteral("Connected to %1").arg(m_carPlayDeviceName);
    } else {
        newCarPlayStatus = m_carPlayWirelessEnabled
            ? QStringLiteral("Wireless ready for %1").arg(m_carPlayDeviceName)
            : QStringLiteral("Plug in %1 via USB").arg(m_carPlayDeviceName);
    }

    if (newCarPlayStatus != m_carPlayStatus) {
        m_carPlayStatus = newCarPlayStatus;
        emit carPlayStatusChanged(m_carPlayStatus);
    }

    QString newAndroidStatus;
    if (!m_androidAutoAvailable) {
        newAndroidStatus = QStringLiteral("Unavailable");
    } else if (m_androidAutoConnected) {
        newAndroidStatus = QStringLiteral("Connected to %1").arg(m_androidAutoDeviceName);
    } else {
        newAndroidStatus = m_androidAutoWirelessEnabled
            ? QStringLiteral("Wireless ready for %1").arg(m_androidAutoDeviceName)
            : QStringLiteral("Plug in %1 via USB").arg(m_androidAutoDeviceName);
    }

    if (newAndroidStatus != m_androidAutoStatus) {
        m_androidAutoStatus = newAndroidStatus;
        emit androidAutoStatusChanged(m_androidAutoStatus);
    }

    if (!m_carPlayConnected && !m_androidAutoConnected)
        setActiveProjection(QStringLiteral("None"));
}

void ProjectionController::updateSessionMock()
{
    auto makeApp = [](const QString& name, const QString& icon, const QString& status, const QString& accent) {
        QVariantMap map;
        map.insert(QStringLiteral("name"), name);
        map.insert(QStringLiteral("icon"), icon);
        map.insert(QStringLiteral("status"), status);
        map.insert(QStringLiteral("accent"), accent);
        return map;
    };

    auto makeNavTile = [](const QString& title, const QString& instruction, const QString& eta, const QString& detail, const QString& accent, const QString& icon) {
        QVariantMap map;
        map.insert(QStringLiteral("title"), title);
        map.insert(QStringLiteral("instruction"), instruction);
        map.insert(QStringLiteral("eta"), eta);
        map.insert(QStringLiteral("detail"), detail);
        map.insert(QStringLiteral("accent"), accent);
        map.insert(QStringLiteral("icon"), icon);
        return map;
    };

    auto makeMediaTile = [](const QString& title, const QString& artist, const QString& album, const QString& accent, const QString& artworkEmoji) {
        QVariantMap map;
        map.insert(QStringLiteral("title"), title);
        map.insert(QStringLiteral("artist"), artist);
        map.insert(QStringLiteral("album"), album);
        map.insert(QStringLiteral("accent"), accent);
        map.insert(QStringLiteral("artwork"), artworkEmoji);
        return map;
    };

    QString platform = QStringLiteral("None");
    QString summary = QStringLiteral("Connect a trusted device to launch projection");
    QVariantList apps;
    QVariantMap navTile;
    QVariantMap mediaTile;

    if (m_carPlayConnected) {
        platform = QStringLiteral("Apple CarPlay");
        summary = QStringLiteral("Siri ready · Maps guidance streaming to cluster");
        apps.append(makeApp(QStringLiteral("Maps"), QStringLiteral("🗺"), QStringLiteral("Next turn on Elm St. in 300 ft"), QStringLiteral("#60A5FA")));
        apps.append(makeApp(QStringLiteral("Now Playing"), QStringLiteral("🎵"), QStringLiteral("Midnight City — M83"), QStringLiteral("#F472B6")));
        apps.append(makeApp(QStringLiteral("Phone"), QStringLiteral("📞"), QStringLiteral("Ava's iPhone connected"), QStringLiteral("#A78BFA")));
        apps.append(makeApp(QStringLiteral("Calendar"), QStringLiteral("📅"), QStringLiteral("Design review at 2:00 PM"), QStringLiteral("#22C55E")));
        apps.append(makeApp(QStringLiteral("Messages"), QStringLiteral("💬"), QStringLiteral("New message from Jordan"), QStringLiteral("#F59E0B")));
        apps.append(makeApp(QStringLiteral("Podcasts"), QStringLiteral("🎙"), QStringLiteral("Energy Week · Episode 142"), QStringLiteral("#06B6D4")));
        navTile = makeNavTile(QStringLiteral("Elm Street"),
                              QStringLiteral("Turn right in 300 ft"),
                              QStringLiteral("12 min · 4.3 mi"),
                              QStringLiteral("Traffic moderate · Arrive 2:05 PM"),
                              QStringLiteral("#60A5FA"),
                              QStringLiteral("🡆"));
        mediaTile = makeMediaTile(QStringLiteral("Midnight City"),
                                  QStringLiteral("M83"),
                                  QStringLiteral("Saturdays = Youth"),
                                  QStringLiteral("#F472B6"),
                                  QStringLiteral("🎵"));
    } else if (m_androidAutoConnected) {
        platform = QStringLiteral("Android Auto");
        summary = QStringLiteral("Assistant listening · Split-screen nav + media running");
        apps.append(makeApp(QStringLiteral("Maps"), QStringLiteral("🧭"), QStringLiteral("Toll road ahead in 1.5 mi"), QStringLiteral("#34D399")));
        apps.append(makeApp(QStringLiteral("Media"), QStringLiteral("🎧"), QStringLiteral("Synthwave Drive playlist"), QStringLiteral("#FBBF24")));
        apps.append(makeApp(QStringLiteral("Calls"), QStringLiteral("📱"), QStringLiteral("Pixel 9 Pro connected"), QStringLiteral("#60A5FA")));
        apps.append(makeApp(QStringLiteral("Assistant"), QStringLiteral("🤖"), QStringLiteral("Voice command ready"), QStringLiteral("#A855F7")));
        apps.append(makeApp(QStringLiteral("Notifications"), QStringLiteral("🔔"), QStringLiteral("2 unread alerts"), QStringLiteral("#F87171")));
        apps.append(makeApp(QStringLiteral("Energy"), QStringLiteral("⚡"), QStringLiteral("Battery at 78%"), QStringLiteral("#14B8A6")));
        navTile = makeNavTile(QStringLiteral("US-101"),
                              QStringLiteral("Keep left to stay on US-101"),
                              QStringLiteral("18 min · 7.2 mi"),
                              QStringLiteral("Fastest route despite usual traffic"),
                              QStringLiteral("#34D399"),
                              QStringLiteral("⬏"));
        mediaTile = makeMediaTile(QStringLiteral("Synthwave Drive"),
                                  QStringLiteral("AutoSounds"),
                                  QStringLiteral("Drive Mix"),
                                  QStringLiteral("#FBBF24"),
                                  QStringLiteral("🎧"));
    }

    bool active = platform != QStringLiteral("None");
    bool stateChanged = false;
    if (m_sessionActive != active || m_sessionPlatform != platform || m_sessionSummary != summary) {
        m_sessionActive = active;
        m_sessionPlatform = platform;
        m_sessionSummary = summary;
        stateChanged = true;
    }

    if (m_sessionNavTile != navTile) {
        m_sessionNavTile = navTile;
        stateChanged = true;
    }

    if (m_sessionMediaTile != mediaTile) {
        m_sessionMediaTile = mediaTile;
        stateChanged = true;
    }

    if (stateChanged)
        emit sessionStateChanged();

    if (m_sessionApps != apps) {
        m_sessionApps = apps;
        emit sessionAppsChanged();
    }
}

} // namespace infotainment
} // namespace automotive
