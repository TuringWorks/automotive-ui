// PerceptionModel.h
// Perception Data Model for ADAS Visualization
// Part of: Shared ADAS Platform Layer
// Safety: Validated perception data for UI presentation
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.2

#ifndef AUTOMOTIVE_PERCEPTION_MODEL_H
#define AUTOMOTIVE_PERCEPTION_MODEL_H

#include "AdasTypes.h"
#include <QObject>
#include <QVector>
#include <QMutex>
#include <QElapsedTimer>

namespace automotive {
namespace adas {

/**
 * @brief Lane boundary data
 */
struct LaneBoundary {
    LaneType type{LaneType::Unknown};
    ConfidenceLevel confidence{ConfidenceLevel::NotAvailable};

    // Control points for lane curve (in vehicle coords)
    QVector<Position2D> points;

    // Quality metrics
    double qualityScore{0.0};       ///< 0.0-1.0 overall quality
    bool isValid{false};

    /**
     * @brief Get color hint for visualization based on confidence
     */
    QString confidenceColorHint() const {
        switch (confidence) {
            case ConfidenceLevel::High: return QStringLiteral("green");
            case ConfidenceLevel::Medium: return QStringLiteral("yellow");
            case ConfidenceLevel::Low: return QStringLiteral("red");
            default: return QStringLiteral("gray");
        }
    }
};

/**
 * @brief Lane model data (both boundaries)
 *
 * Core topic: LaneModel
 */
struct LaneModel {
    SignalMetadata metadata;

    LaneBoundary left;
    LaneBoundary right;

    // Lane geometry
    double laneWidthM{3.5};          ///< Estimated lane width
    double curvatureRadM{0.0};       ///< Lane curvature (0 = straight)
    double headingOffsetDeg{0.0};    ///< Vehicle heading offset from lane center

    // Lateral position
    double lateralOffsetM{0.0};      ///< Offset from lane center (+left, -right)
    bool isCentered{true};           ///< Vehicle is reasonably centered

    /**
     * @brief Get overall lane quality for visualization
     *
     * Requirement: SR-CL-ADAS-140, SR-CL-ADAS-141
     */
    ConfidenceLevel overallQuality() const {
        // Take minimum of left and right
        if (!left.isValid || !right.isValid) {
            return ConfidenceLevel::NotAvailable;
        }
        return std::min(left.confidence, right.confidence);
    }

    bool hasValidLanes() const {
        return left.isValid || right.isValid;
    }
};

/**
 * @brief Detected object data
 */
struct PerceivedObject {
    uint32_t id{0};                  ///< Tracking ID
    ObjectType type{ObjectType::Unknown};
    ConfidenceLevel confidence{ConfidenceLevel::NotAvailable};

    // Position (vehicle coords)
    Position3D position;
    double heading{0.0};             ///< Object heading in degrees

    // Kinematics
    double speedMps{0.0};            ///< Absolute speed m/s
    double relativeSpeedMps{0.0};    ///< Relative to ego vehicle

    // Bounding box
    double lengthM{0.0};
    double widthM{0.0};
    double heightM{0.0};

    // For visualization
    bool isRelevant{false};          ///< In our lane or adjacent
    bool isThreat{false};            ///< Potential collision risk

    double distanceM() const {
        return std::sqrt(position.x * position.x + position.y * position.y);
    }
};

/**
 * @brief Lead vehicle data
 *
 * Core topic: LeadVehicle
 */
struct LeadVehicle {
    SignalMetadata metadata;

    uint32_t id{0};                  ///< Tracking ID
    bool detected{false};            ///< Lead vehicle present
    ConfidenceLevel confidence{ConfidenceLevel::NotAvailable};

    // Position
    double distanceM{0.0};           ///< Distance to lead vehicle
    double lateralOffsetM{0.0};      ///< Lateral offset from center

    // Kinematics
    double relativeSpeedMps{0.0};    ///< Negative = closing
    double absoluteSpeedMps{0.0};    ///< Lead vehicle speed

    // Time-based metrics
    double timeGapSec{0.0};          ///< Time gap to lead
    double ttcSec{0.0};              ///< Time-to-collision (-1 = diverging)

    bool isClosing() const { return relativeSpeedMps < -0.5; }
    bool isDiverging() const { return relativeSpeedMps > 0.5; }
};

/**
 * @brief Drivable corridor / planned path
 *
 * Core topic: DrivableCorridor
 */
struct DrivableCorridor {
    SignalMetadata metadata;

    // Path centerline (vehicle coords)
    QVector<Position2D> centerline;

    // Corridor boundaries
    QVector<Position2D> leftBoundary;
    QVector<Position2D> rightBoundary;

    // Path properties
    double corridorWidthM{2.0};
    double pathLengthM{100.0};
    double maxCurvature{0.0};

    bool isValid{false};
};

/**
 * @brief Object list container
 *
 * Core topic: ObjectList
 */
struct ObjectList {
    SignalMetadata metadata;

    QVector<PerceivedObject> objects;

    int vehicleCount() const {
        int count = 0;
        for (const auto& obj : objects) {
            if (obj.type == ObjectType::Car || obj.type == ObjectType::Truck ||
                obj.type == ObjectType::Motorcycle) {
                ++count;
            }
        }
        return count;
    }

    int pedestrianCount() const {
        int count = 0;
        for (const auto& obj : objects) {
            if (obj.type == ObjectType::Pedestrian) {
                ++count;
            }
        }
        return count;
    }

    QVector<PerceivedObject> getRelevantObjects() const {
        QVector<PerceivedObject> relevant;
        for (const auto& obj : objects) {
            if (obj.isRelevant) {
                relevant.append(obj);
            }
        }
        return relevant;
    }
};

/**
 * @brief Perception Model Service
 *
 * Provides validated perception data for visualization.
 *
 * Safety Requirements:
 * - SR-CL-ADAS-140: Display DEGRADED within 300ms on confidence drop
 * - SR-CL-ADAS-141: Lane visualization reflects quality
 * - SR-CL-ADAS-142: Hide stale object markers
 */
class PerceptionModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool hasValidLanes READ hasValidLanes NOTIFY laneModelChanged)
    Q_PROPERTY(int laneQuality READ laneQualityInt NOTIFY laneModelChanged)
    Q_PROPERTY(bool hasLeadVehicle READ hasLeadVehicle NOTIFY leadVehicleChanged)
    Q_PROPERTY(double leadDistance READ leadDistance NOTIFY leadVehicleChanged)
    Q_PROPERTY(double leadRelativeSpeed READ leadRelativeSpeed NOTIFY leadVehicleChanged)
    Q_PROPERTY(double timeGap READ timeGap NOTIFY leadVehicleChanged)
    Q_PROPERTY(int objectCount READ objectCount NOTIFY objectListChanged)
    Q_PROPERTY(double lateralOffset READ lateralOffset NOTIFY laneModelChanged)

public:
    explicit PerceptionModel(QObject* parent = nullptr);
    ~PerceptionModel() override;

    // Lane model access
    bool hasValidLanes() const;
    ConfidenceLevel laneQuality() const;
    int laneQualityInt() const { return static_cast<int>(laneQuality()); }
    double lateralOffset() const;
    LaneModel laneModel() const;

    // Lead vehicle access
    bool hasLeadVehicle() const;
    double leadDistance() const;
    double leadRelativeSpeed() const;
    double timeGap() const;
    LeadVehicle leadVehicle() const;

    // Object list access
    int objectCount() const;
    ObjectList objectList() const;
    QVector<PerceivedObject> relevantObjects() const;

    // Corridor access
    bool hasValidCorridor() const;
    DrivableCorridor corridor() const;

    // Updates from IPC
    bool updateLaneModel(const LaneModel& lanes);
    bool updateLeadVehicle(const LeadVehicle& lead);
    bool updateObjectList(const ObjectList& objects);
    bool updateCorridor(const DrivableCorridor& corridor);

    // Tick processing
    void processTick(uint64_t tickNumber, qint64 elapsedMs);

    static constexpr qint64 PERCEPTION_FRESHNESS_MS = 200;

signals:
    void laneModelChanged();
    void leadVehicleChanged();
    void objectListChanged();
    void corridorChanged();
    void perceptionDegraded(const QString& reason);

private:
    void checkFreshness(qint64 currentTimeMs);
    qint64 currentMonotonicTimeMs() const;

    mutable QMutex m_mutex;

    LaneModel m_laneModel;
    LeadVehicle m_leadVehicle;
    ObjectList m_objectList;
    DrivableCorridor m_corridor;

    qint64 m_lastLaneUpdateMs{0};
    qint64 m_lastLeadUpdateMs{0};
    qint64 m_lastObjectUpdateMs{0};
    qint64 m_lastCorridorUpdateMs{0};

    QElapsedTimer m_monotonicTimer;
};

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::LaneModel)
Q_DECLARE_METATYPE(automotive::adas::LeadVehicle)
Q_DECLARE_METATYPE(automotive::adas::ObjectList)
Q_DECLARE_METATYPE(automotive::adas::DrivableCorridor)

#endif // AUTOMOTIVE_PERCEPTION_MODEL_H
