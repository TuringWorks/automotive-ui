// PerceptionModel.cpp
// Perception Data Model Implementation
// Safety: Validated perception data with freshness checking

#include "PerceptionModel.h"
#include <QMutexLocker>

namespace automotive {
namespace adas {

PerceptionModel::PerceptionModel(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
}

PerceptionModel::~PerceptionModel() = default;

qint64 PerceptionModel::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

bool PerceptionModel::hasValidLanes() const
{
    QMutexLocker locker(&m_mutex);
    return m_laneModel.hasValidLanes() && m_laneModel.metadata.isValid();
}

ConfidenceLevel PerceptionModel::laneQuality() const
{
    QMutexLocker locker(&m_mutex);
    return m_laneModel.overallQuality();
}

double PerceptionModel::lateralOffset() const
{
    QMutexLocker locker(&m_mutex);
    return m_laneModel.lateralOffsetM;
}

LaneModel PerceptionModel::laneModel() const
{
    QMutexLocker locker(&m_mutex);
    return m_laneModel;
}

bool PerceptionModel::hasLeadVehicle() const
{
    QMutexLocker locker(&m_mutex);
    return m_leadVehicle.detected && m_leadVehicle.metadata.isValid();
}

double PerceptionModel::leadDistance() const
{
    QMutexLocker locker(&m_mutex);
    return m_leadVehicle.distanceM;
}

double PerceptionModel::leadRelativeSpeed() const
{
    QMutexLocker locker(&m_mutex);
    return m_leadVehicle.relativeSpeedMps;
}

double PerceptionModel::timeGap() const
{
    QMutexLocker locker(&m_mutex);
    return m_leadVehicle.timeGapSec;
}

LeadVehicle PerceptionModel::leadVehicle() const
{
    QMutexLocker locker(&m_mutex);
    return m_leadVehicle;
}

int PerceptionModel::objectCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_objectList.objects.size();
}

ObjectList PerceptionModel::objectList() const
{
    QMutexLocker locker(&m_mutex);
    return m_objectList;
}

QVector<PerceivedObject> PerceptionModel::relevantObjects() const
{
    QMutexLocker locker(&m_mutex);
    return m_objectList.getRelevantObjects();
}

bool PerceptionModel::hasValidCorridor() const
{
    QMutexLocker locker(&m_mutex);
    return m_corridor.isValid && m_corridor.metadata.isValid();
}

DrivableCorridor PerceptionModel::corridor() const
{
    QMutexLocker locker(&m_mutex);
    return m_corridor;
}

bool PerceptionModel::updateLaneModel(const LaneModel& lanes)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    {
        QMutexLocker locker(&m_mutex);
        m_laneModel = lanes;
        m_lastLaneUpdateMs = currentTime;
    }

    emit laneModelChanged();
    return true;
}

bool PerceptionModel::updateLeadVehicle(const LeadVehicle& lead)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    {
        QMutexLocker locker(&m_mutex);
        m_leadVehicle = lead;
        m_lastLeadUpdateMs = currentTime;
    }

    emit leadVehicleChanged();
    return true;
}

bool PerceptionModel::updateObjectList(const ObjectList& objects)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    {
        QMutexLocker locker(&m_mutex);
        m_objectList = objects;
        m_lastObjectUpdateMs = currentTime;
    }

    emit objectListChanged();
    return true;
}

bool PerceptionModel::updateCorridor(const DrivableCorridor& corridor)
{
    const qint64 currentTime = currentMonotonicTimeMs();

    {
        QMutexLocker locker(&m_mutex);
        m_corridor = corridor;
        m_lastCorridorUpdateMs = currentTime;
    }

    emit corridorChanged();
    return true;
}

void PerceptionModel::processTick(uint64_t /*tickNumber*/, qint64 elapsedMs)
{
    checkFreshness(elapsedMs);
}

void PerceptionModel::checkFreshness(qint64 currentTimeMs)
{
    QMutexLocker locker(&m_mutex);

    // Check lane freshness - SR-CL-ADAS-140
    if (m_lastLaneUpdateMs > 0 &&
        (currentTimeMs - m_lastLaneUpdateMs) > PERCEPTION_FRESHNESS_MS) {
        if (m_laneModel.metadata.valid) {
            m_laneModel.metadata.valid = false;
            locker.unlock();
            emit perceptionDegraded(QStringLiteral("Lane detection stale"));
            emit laneModelChanged();
            locker.relock();
        }
    }

    // Check lead vehicle freshness
    if (m_lastLeadUpdateMs > 0 &&
        (currentTimeMs - m_lastLeadUpdateMs) > PERCEPTION_FRESHNESS_MS) {
        if (m_leadVehicle.metadata.valid) {
            m_leadVehicle.metadata.valid = false;
            m_leadVehicle.detected = false;
            locker.unlock();
            emit leadVehicleChanged();
            locker.relock();
        }
    }

    // Check object list freshness - SR-CL-ADAS-142
    if (m_lastObjectUpdateMs > 0 &&
        (currentTimeMs - m_lastObjectUpdateMs) > PERCEPTION_FRESHNESS_MS) {
        if (m_objectList.metadata.valid) {
            m_objectList.metadata.valid = false;
            m_objectList.objects.clear(); // Hide stale markers
            locker.unlock();
            emit objectListChanged();
            locker.relock();
        }
    }

    // Check corridor freshness
    if (m_lastCorridorUpdateMs > 0 &&
        (currentTimeMs - m_lastCorridorUpdateMs) > PERCEPTION_FRESHNESS_MS) {
        if (m_corridor.metadata.valid) {
            m_corridor.metadata.valid = false;
            m_corridor.isValid = false;
            locker.unlock();
            emit corridorChanged();
        }
    }
}

} // namespace adas
} // namespace automotive
