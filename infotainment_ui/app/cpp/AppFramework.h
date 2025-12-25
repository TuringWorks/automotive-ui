// AppFramework.h
// Infotainment app framework
// Manages navigation and app lifecycle

#ifndef AUTOMOTIVE_APP_FRAMEWORK_H
#define AUTOMOTIVE_APP_FRAMEWORK_H

#include <QObject>
#include <QStack>
#include <QString>

namespace automotive {
namespace infotainment {

class PermissionManager;
class SecureSettings;

/**
 * @brief App pages/screens
 */
enum class AppPage {
    Home,
    Media,
    Navigation,
    Phone,
    Projection,
    Vehicle,
    Settings,
    AppDrawer
};

/**
 * @brief App framework for navigation and state management
 */
class AppFramework : public QObject {
    Q_OBJECT

    Q_PROPERTY(int currentPage READ currentPageInt NOTIFY currentPageChanged)
    Q_PROPERTY(QString currentPageName READ currentPageName NOTIFY currentPageChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY navigationChanged)
    Q_PROPERTY(bool driverDistractionMode READ driverDistractionMode NOTIFY driverDistractionModeChanged)

public:
    explicit AppFramework(QObject* parent = nullptr);
    ~AppFramework() override;

    void setPermissionManager(PermissionManager* manager);
    void setSecureSettings(SecureSettings* settings);

    AppPage currentPage() const { return m_currentPage; }
    int currentPageInt() const { return static_cast<int>(m_currentPage); }
    QString currentPageName() const;
    bool canGoBack() const { return !m_navigationStack.isEmpty(); }

    bool driverDistractionMode() const { return m_driverDistractionMode; }

public slots:
    /**
     * @brief Navigate to a page
     * @param page Page index
     */
    void navigateTo(int page);

    /**
     * @brief Navigate to a page by name
     */
    void navigateToPage(const QString& pageName);

    /**
     * @brief Go back to previous page
     */
    void goBack();

    /**
     * @brief Go to home page
     */
    void goHome();

    /**
     * @brief Set driver distraction mode
     * @param enabled If true, distracting features are disabled
     */
    void setDriverDistractionMode(bool enabled);

signals:
    void currentPageChanged(int page);
    void navigationChanged();
    void driverDistractionModeChanged(bool enabled);

private:
    AppPage stringToPage(const QString& name) const;
    QString pageToString(AppPage page) const;

    AppPage m_currentPage{AppPage::Home};
    QStack<AppPage> m_navigationStack;
    bool m_driverDistractionMode{false};

    PermissionManager* m_permissionManager{nullptr};
    SecureSettings* m_secureSettings{nullptr};
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_APP_FRAMEWORK_H
