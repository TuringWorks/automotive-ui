// NavigationController.h
// Navigation controller (mock/stub)

#ifndef AUTOMOTIVE_NAVIGATION_CONTROLLER_H
#define AUTOMOTIVE_NAVIGATION_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoPositionInfoSource>

namespace automotive {
namespace infotainment {

/**
 * @brief Navigation controller (stub for v1)
 */
class NavigationController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QString destination READ destination NOTIFY destinationChanged)
    Q_PROPERTY(QString nextInstruction READ nextInstruction NOTIFY instructionChanged)
    Q_PROPERTY(int distanceToTurn READ distanceToTurn NOTIFY instructionChanged)
    Q_PROPERTY(QString eta READ eta NOTIFY etaChanged)
    Q_PROPERTY(int remainingDistance READ remainingDistance NOTIFY etaChanged)

    // Map properties
    Q_PROPERTY(double latitude READ latitude NOTIFY positionChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY positionChanged)
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(double heading READ heading NOTIFY positionChanged)
    Q_PROPERTY(QVariantList routePath READ routePath NOTIFY routeChanged)
    Q_PROPERTY(QVariantList recentDestinations READ recentDestinations NOTIFY recentDestinationsChanged)

public:
    explicit NavigationController(QObject* parent = nullptr);
    ~NavigationController() override;

    bool isActive() const { return m_active; }
    QString destination() const { return m_destination; }
    QString nextInstruction() const { return m_nextInstruction; }
    int distanceToTurn() const { return m_distanceToTurn; }
    QString eta() const { return m_eta; }
    int remainingDistance() const { return m_remainingDistance; }

    // Map getters
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double zoomLevel() const { return m_zoomLevel; }
    double heading() const { return m_heading; }
    QVariantList routePath() const { return m_routePath; }
    QVariantList recentDestinations() const { return m_recentDestinations; }

public slots:
    void startNavigation(const QString& destination);
    void stopNavigation();
    void recalculateRoute();
    void setZoomLevel(double zoom);
    void zoomIn();
    void zoomOut();
    void centerOnPosition();

signals:
    void activeChanged(bool active);
    void destinationChanged(const QString& destination);
    void instructionChanged();
    void etaChanged();
    void positionChanged();
    void zoomLevelChanged();
    void routeChanged();
    void recentDestinationsChanged();

private slots:
    void onPositionTick();
    void onGpsPositionUpdated(const QGeoPositionInfo& info);

private:
    void loadRecentDestinations();
    void setupMockRoute();

    bool m_active{false};
    QString m_destination;
    QString m_nextInstruction{QStringLiteral("Turn right in 500m")};
    int m_distanceToTurn{500};
    QString m_eta{QStringLiteral("12:45")};
    int m_remainingDistance{15000};

    // Map state - Default: San Francisco area
    double m_latitude{40.01499};
    double m_longitude{-105.27055};
    double m_zoomLevel{14.0};
    double m_heading{0.0};
    QVariantList m_routePath;
    QVariantList m_recentDestinations;
    QTimer* m_positionTimer{nullptr};
    int m_routeIndex{0};

    QGeoPositionInfoSource* m_positionSource{nullptr};
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_NAVIGATION_CONTROLLER_H
