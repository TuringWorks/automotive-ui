// NavigationController.cpp
// Navigation controller implementation

#include "NavigationController.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>

namespace automotive {
namespace infotainment {

NavigationController::NavigationController(QObject* parent)
    : QObject(parent)
{
    m_positionTimer = new QTimer(this);
    m_positionTimer->setInterval(2000);  // Update every 2 seconds
    connect(m_positionTimer, &QTimer::timeout, this, &NavigationController::onPositionTick);

    loadRecentDestinations();

    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_positionSource) {
        m_positionSource->setUpdateInterval(2000);
        connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated,
                this, &NavigationController::onGpsPositionUpdated);
        m_positionSource->startUpdates();
    }
}

NavigationController::~NavigationController() = default;

void NavigationController::startNavigation(const QString& destination)
{
    m_destination = destination;
    m_active = true;
    m_routeIndex = 0;

    setupMockRoute();

    m_positionTimer->start();

    emit destinationChanged(m_destination);
    emit activeChanged(true);
    emit routeChanged();
}

void NavigationController::stopNavigation()
{
    m_active = false;
    m_destination.clear();
    m_positionTimer->stop();
    m_routePath.clear();

    emit activeChanged(false);
    emit destinationChanged(QString());
    emit routeChanged();
}

void NavigationController::recalculateRoute()
{
    // Mock: just emit signals
    emit instructionChanged();
    emit etaChanged();
}

void NavigationController::setZoomLevel(double zoom)
{
    zoom = qBound(10.0, zoom, 18.0);
    if (m_zoomLevel != zoom) {
        m_zoomLevel = zoom;
        emit zoomLevelChanged();
    }
}

void NavigationController::zoomIn()
{
    setZoomLevel(m_zoomLevel + 1.0);
}

void NavigationController::zoomOut()
{
    setZoomLevel(m_zoomLevel - 1.0);
}

void NavigationController::centerOnPosition()
{
    // Just emit signal to trigger map recentering
    emit positionChanged();
}

void NavigationController::onPositionTick()
{
    if (!m_active || m_routePath.isEmpty()) {
        return;
    }

    // Move along route
    m_routeIndex++;
    if (m_routeIndex >= m_routePath.size()) {
        m_routeIndex = m_routePath.size() - 1;
        m_positionTimer->stop();

        // Arrived
        m_nextInstruction = QStringLiteral("You have arrived");
        m_distanceToTurn = 0;
        m_remainingDistance = 0;
        emit instructionChanged();
        emit etaChanged();
        return;
    }

    QVariantMap point = m_routePath.at(m_routeIndex).toMap();
    m_latitude = point.value(QStringLiteral("lat")).toDouble();
    m_longitude = point.value(QStringLiteral("lng")).toDouble();

    // Calculate heading from previous point
    if (m_routeIndex > 0) {
        QVariantMap prevPoint = m_routePath.at(m_routeIndex - 1).toMap();
        double prevLat = prevPoint.value(QStringLiteral("lat")).toDouble();
        double prevLng = prevPoint.value(QStringLiteral("lng")).toDouble();
        double dLng = m_longitude - prevLng;
        double dLat = m_latitude - prevLat;
        m_heading = qAtan2(dLng, dLat) * 180.0 / M_PI;
    }

    // Update remaining distance (mock)
    m_remainingDistance = (m_routePath.size() - m_routeIndex) * 100;

    // Update distance to turn (mock)
    m_distanceToTurn = qMax(0, m_distanceToTurn - 50);
    if (m_distanceToTurn <= 0) {
        m_distanceToTurn = 300 + (QRandomGenerator::global()->bounded(500));
        // Cycle through instructions
        static int instrIndex = 0;
        QStringList instructions = {
            QStringLiteral("Turn right onto Canyon Blvd"),
            QStringLiteral("Continue on Broadway for 0.6 mi"),
            QStringLiteral("Turn left toward Walnut St"),
            QStringLiteral("Merge onto CO-119 toward Lyons"),
            QStringLiteral("Take exit toward 28th St")
        };
        m_nextInstruction = instructions.at(instrIndex % instructions.size());
        instrIndex++;
        emit instructionChanged();
    }

    emit positionChanged();
    emit etaChanged();
}

void NavigationController::onGpsPositionUpdated(const QGeoPositionInfo& info)
{
    if (!info.isValid())
        return;

    const QGeoCoordinate coord = info.coordinate();
    if (coord.isValid()) {
        bool changed = false;
        if (!qFuzzyCompare(m_latitude, coord.latitude()) || !qFuzzyCompare(m_longitude, coord.longitude())) {
            m_latitude = coord.latitude();
            m_longitude = coord.longitude();
            changed = true;
        }

        if (info.hasAttribute(QGeoPositionInfo::Direction)) {
            double newHeading = info.attribute(QGeoPositionInfo::Direction);
            if (!qFuzzyCompare(m_heading, newHeading)) {
                m_heading = newHeading;
                changed = true;
            }
        }

        if (changed)
            emit positionChanged();
    }
}

void NavigationController::loadRecentDestinations()
{
    m_recentDestinations.clear();

    auto addDest = [this](const QString& name, const QString& address,
                          double lat, double lng) {
        QVariantMap dest;
        dest[QStringLiteral("name")] = name;
        dest[QStringLiteral("address")] = address;
        dest[QStringLiteral("lat")] = lat;
        dest[QStringLiteral("lng")] = lng;
        m_recentDestinations.append(dest);
    };

    addDest(QStringLiteral("Home"),
            QStringLiteral("331 Canyon Blvd, Boulder"),
            40.0175, -105.2797);

    addDest(QStringLiteral("Work"),
            QStringLiteral("1900 Pearl St, Boulder"),
            40.0205, -105.2700);

    addDest(QStringLiteral("Chautauqua Park"),
            QStringLiteral("Baseline Rd, Boulder"),
            39.9981, -105.2817);

    addDest(QStringLiteral("Denver Intl Airport"),
            QStringLiteral("8500 Peña Blvd, Denver"),
            39.8561, -104.6737);

    addDest(QStringLiteral("Pearl Street Mall"),
            QStringLiteral("1300 Pearl St, Boulder"),
            40.0195, -105.2730);

    emit recentDestinationsChanged();
}

void NavigationController::setupMockRoute()
{
    m_routePath.clear();

    auto addPoint = [this](double lat, double lng) {
        QVariantMap point;
        point[QStringLiteral("lat")] = lat;
        point[QStringLiteral("lng")] = lng;
        m_routePath.append(point);
    };

    // Create a mock route around Boulder
    // Starting from current position going towards destination
    double startLat = m_latitude;
    double startLng = m_longitude;

    // Generate 20 points along a mock route
    for (int i = 0; i < 20; ++i) {
        double progress = static_cast<double>(i) / 19.0;

        // Add some curves to make it look like a real route
        double lat = startLat + (0.008 * progress) + (0.003 * qSin(progress * M_PI * 2));
        double lng = startLng + (0.01 * progress) + (0.002 * qCos(progress * M_PI * 3));

        addPoint(lat, lng);
    }

    emit routeChanged();
}

} // namespace infotainment
} // namespace automotive
