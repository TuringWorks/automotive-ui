// AppFramework.cpp
// App framework implementation

#include "AppFramework.h"
#include "PermissionManager.h"
#include "SecureSettings.h"

namespace automotive {
namespace infotainment {

AppFramework::AppFramework(QObject* parent)
    : QObject(parent)
{
}

AppFramework::~AppFramework() = default;

void AppFramework::setPermissionManager(PermissionManager* manager)
{
    m_permissionManager = manager;
}

void AppFramework::setSecureSettings(SecureSettings* settings)
{
    m_secureSettings = settings;
}

QString AppFramework::currentPageName() const
{
    return pageToString(m_currentPage);
}

void AppFramework::navigateTo(int page)
{
    AppPage newPage = static_cast<AppPage>(page);
    if (newPage != m_currentPage) {
        m_navigationStack.push(m_currentPage);
        m_currentPage = newPage;
        emit currentPageChanged(static_cast<int>(m_currentPage));
        emit navigationChanged();
    }
}

void AppFramework::navigateToPage(const QString& pageName)
{
    navigateTo(static_cast<int>(stringToPage(pageName)));
}

void AppFramework::goBack()
{
    if (!m_navigationStack.isEmpty()) {
        m_currentPage = m_navigationStack.pop();
        emit currentPageChanged(static_cast<int>(m_currentPage));
        emit navigationChanged();
    }
}

void AppFramework::goHome()
{
    if (m_currentPage != AppPage::Home) {
        m_navigationStack.clear();
        m_currentPage = AppPage::Home;
        emit currentPageChanged(static_cast<int>(m_currentPage));
        emit navigationChanged();
    }
}

void AppFramework::setDriverDistractionMode(bool enabled)
{
    if (m_driverDistractionMode != enabled) {
        m_driverDistractionMode = enabled;
        emit driverDistractionModeChanged(enabled);
    }
}

AppPage AppFramework::stringToPage(const QString& name) const
{
    if (name == QLatin1String("Home")) return AppPage::Home;
    if (name == QLatin1String("Media")) return AppPage::Media;
    if (name == QLatin1String("Navigation")) return AppPage::Navigation;
    if (name == QLatin1String("Phone")) return AppPage::Phone;
    if (name == QLatin1String("Projection")) return AppPage::Projection;
    if (name == QLatin1String("Vehicle")) return AppPage::Vehicle;
    if (name == QLatin1String("Settings")) return AppPage::Settings;
    if (name == QLatin1String("AppDrawer")) return AppPage::AppDrawer;
    return AppPage::Home;
}

QString AppFramework::pageToString(AppPage page) const
{
    switch (page) {
    case AppPage::Home: return QStringLiteral("Home");
    case AppPage::Media: return QStringLiteral("Media");
    case AppPage::Navigation: return QStringLiteral("Navigation");
    case AppPage::Phone: return QStringLiteral("Phone");
    case AppPage::Projection: return QStringLiteral("Projection");
    case AppPage::Vehicle: return QStringLiteral("Vehicle");
    case AppPage::Settings: return QStringLiteral("Settings");
    case AppPage::AppDrawer: return QStringLiteral("AppDrawer");
    }
    return QStringLiteral("Home");
}

} // namespace infotainment
} // namespace automotive
