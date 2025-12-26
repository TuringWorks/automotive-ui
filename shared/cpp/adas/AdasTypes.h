// AdasTypes.h
// Core ADAS type definitions
// Part of: Shared ADAS Platform Layer
// Safety: Type definitions for safety-critical ADAS state management
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md

#ifndef AUTOMOTIVE_ADAS_TYPES_H
#define AUTOMOTIVE_ADAS_TYPES_H

#include <QObject>
#include <QString>
#include <QMetaType>
#include <cstdint>

namespace automotive {
namespace adas {

/**
 * @brief SAE Automation Level
 *
 * Requirement: ADAS_Autonomy_UI_Spec Section 2.1
 * Maps to SAE J3016 taxonomy.
 */
enum class AutomationLevel : uint8_t {
    L0_NoAutomation = 0,    ///< No driving automation
    L1_DriverAssistance,    ///< Either steering OR accel/brake (ACC or LKAS)
    L2_PartialAutomation,   ///< Both steering AND accel/brake (driver supervises)
    L3_ConditionalAutomation, ///< System drives in ODD; user is fallback-ready
    L4_HighAutomation,      ///< System drives and handles fallback in ODD
    L5_FullAutomation       ///< System drives everywhere
};

/**
 * @brief ADAS HMI State Machine
 *
 * Requirement: ADAS_Autonomy_UI_Spec Section 4.2
 * SR-CL-ADAS-100: Never show ENGAGED unless engaged=true AND available=true
 */
enum class AdasHmiState : uint8_t {
    Off = 0,                ///< ADAS_OFF: System not active
    Available,              ///< ADAS_AVAILABLE: Ready to engage
    EngagedL1,              ///< ADAS_ENGAGED_L1: L1 driver assistance active
    EngagedL2,              ///< ADAS_ENGAGED_L2: L2 partial automation active
    AvailableL3,            ///< ADS_AVAILABLE_L3: L3 ready in ODD
    EngagedL3,              ///< ADS_ENGAGED_L3: L3 conditional automation active
    TakeoverRequested,      ///< TAKEOVER_REQUESTED: Driver must take control
    MrmActive,              ///< MRM_ACTIVE: Minimal Risk Maneuver in progress
    Degraded,               ///< DEGRADED: Reduced capability due to sensor issues
    FaultLockout            ///< FAULT_LOCKOUT: System fault, service required
};

/**
 * @brief Takeover request urgency levels
 *
 * Requirement: SR-CL-ADAS-121
 */
enum class TakeoverUrgency : uint8_t {
    None = 0,               ///< No takeover requested
    Advisory,               ///< ODD ending soon, prepare to take over
    Warning,                ///< Take over recommended within seconds
    Immediate,              ///< Take over required NOW
    Critical                ///< Emergency - system initiating MRM if no response
};

/**
 * @brief Lane boundary type
 */
enum class LaneType : uint8_t {
    Unknown = 0,
    Solid,
    Dashed,
    DoubleSolid,
    DashedSolid,
    SolidDashed,
    RoadEdge,
    Virtual                 ///< Computed/predicted boundary
};

/**
 * @brief Perception object type
 */
enum class ObjectType : uint8_t {
    Unknown = 0,
    Car,
    Truck,
    Motorcycle,
    Bicycle,
    Pedestrian,
    Animal,
    Cone,
    Barrier,
    Sign,
    TrafficLight
};

/**
 * @brief Signal confidence level
 *
 * Used for perception and sensor data quality indication.
 */
enum class ConfidenceLevel : uint8_t {
    NotAvailable = 0,       ///< No data
    Low,                    ///< <50% confidence
    Medium,                 ///< 50-80% confidence
    High                    ///< >80% confidence
};

/**
 * @brief ODD (Operational Design Domain) status
 */
enum class OddStatus : uint8_t {
    Unknown = 0,
    Inside,                 ///< Vehicle is within ODD boundaries
    ApproachingBoundary,    ///< Approaching ODD limit
    Outside,                ///< Vehicle is outside ODD
    Degraded                ///< ODD determination is uncertain
};

/**
 * @brief Driver monitoring attention state
 */
enum class AttentionState : uint8_t {
    Unknown = 0,
    Attentive,              ///< Driver is alert and watching road
    Distracted,             ///< Driver attention diverted
    Drowsy,                 ///< Signs of fatigue detected
    Unresponsive            ///< No response to prompts
};

/**
 * @brief Hands-on-wheel state
 */
enum class HandsOnState : uint8_t {
    Unknown = 0,
    HandsOn,                ///< Driver has hands on steering wheel
    HandsOff,               ///< Hands not detected on wheel
    SingleHand              ///< One hand on wheel
};

/**
 * @brief Weather condition for ODD/visualization
 */
enum class WeatherCondition : uint8_t {
    Clear = 0,
    Cloudy,
    LightRain,
    HeavyRain,
    LightSnow,
    HeavySnow,
    Fog,
    Mist,
    Ice
};

/**
 * @brief Environment preset for visualization
 *
 * Requirement: ADAS_Autonomy_UI_Spec Section 10.2
 */
enum class EnvironmentPreset : uint8_t {
    Dawn = 0,
    Day,
    Dusk,
    Night
};

/**
 * @brief Common signal metadata
 *
 * All ADAS signals include this metadata.
 * Requirement: ADAS_Autonomy_UI_Spec Section 3.2
 */
struct SignalMetadata {
    qint64 timestampMs{0};          ///< Monotonic timestamp
    qint64 sourceTimestampMs{0};    ///< Source-provided timestamp
    uint32_t sequenceNumber{0};     ///< Message sequence number
    ConfidenceLevel confidence{ConfidenceLevel::NotAvailable};
    bool valid{false};              ///< Overall validity flag

    bool isValid() const { return valid && confidence >= ConfidenceLevel::Low; }
    bool isFresh(qint64 currentTimeMs, qint64 freshnessWindowMs = 300) const {
        return (currentTimeMs - timestampMs) <= freshnessWindowMs;
    }
};

/**
 * @brief 2D position in vehicle coordinate frame
 */
struct Position2D {
    double x{0.0};          ///< Forward distance (meters)
    double y{0.0};          ///< Lateral offset (meters, positive = left)
};

/**
 * @brief 3D position in vehicle coordinate frame
 */
struct Position3D {
    double x{0.0};          ///< Forward distance (meters)
    double y{0.0};          ///< Lateral offset (meters)
    double z{0.0};          ///< Height (meters)
};

/**
 * @brief Convert automation level to display string
 */
inline QString automationLevelToString(AutomationLevel level) {
    switch (level) {
        case AutomationLevel::L0_NoAutomation: return QStringLiteral("No Automation");
        case AutomationLevel::L1_DriverAssistance: return QStringLiteral("Driver Assistance");
        case AutomationLevel::L2_PartialAutomation: return QStringLiteral("Partial Automation");
        case AutomationLevel::L3_ConditionalAutomation: return QStringLiteral("Conditional Automation");
        case AutomationLevel::L4_HighAutomation: return QStringLiteral("High Automation");
        case AutomationLevel::L5_FullAutomation: return QStringLiteral("Full Automation");
    }
    return QStringLiteral("Unknown");
}

/**
 * @brief Convert HMI state to display string
 */
inline QString adasHmiStateToString(AdasHmiState state) {
    switch (state) {
        case AdasHmiState::Off: return QStringLiteral("Off");
        case AdasHmiState::Available: return QStringLiteral("Available");
        case AdasHmiState::EngagedL1: return QStringLiteral("Engaged (L1)");
        case AdasHmiState::EngagedL2: return QStringLiteral("Engaged (L2)");
        case AdasHmiState::AvailableL3: return QStringLiteral("Available (L3)");
        case AdasHmiState::EngagedL3: return QStringLiteral("Engaged (L3)");
        case AdasHmiState::TakeoverRequested: return QStringLiteral("Takeover Requested");
        case AdasHmiState::MrmActive: return QStringLiteral("Safe Stop Active");
        case AdasHmiState::Degraded: return QStringLiteral("Limited");
        case AdasHmiState::FaultLockout: return QStringLiteral("Service Required");
    }
    return QStringLiteral("Unknown");
}

/**
 * @brief Get responsibility owner text for automation level
 *
 * Requirement: SG-ADAS-003 - Always communicate responsibility owner
 */
inline QString responsibilityOwner(AutomationLevel level, bool engaged) {
    if (!engaged) {
        return QStringLiteral("Driver");
    }
    switch (level) {
        case AutomationLevel::L0_NoAutomation:
        case AutomationLevel::L1_DriverAssistance:
        case AutomationLevel::L2_PartialAutomation:
            return QStringLiteral("Driver (System Assists)");
        case AutomationLevel::L3_ConditionalAutomation:
            return QStringLiteral("System (Be Ready to Take Over)");
        case AutomationLevel::L4_HighAutomation:
        case AutomationLevel::L5_FullAutomation:
            return QStringLiteral("System");
    }
    return QStringLiteral("Driver");
}

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::AutomationLevel)
Q_DECLARE_METATYPE(automotive::adas::AdasHmiState)
Q_DECLARE_METATYPE(automotive::adas::TakeoverUrgency)
Q_DECLARE_METATYPE(automotive::adas::LaneType)
Q_DECLARE_METATYPE(automotive::adas::ObjectType)
Q_DECLARE_METATYPE(automotive::adas::ConfidenceLevel)
Q_DECLARE_METATYPE(automotive::adas::OddStatus)
Q_DECLARE_METATYPE(automotive::adas::AttentionState)
Q_DECLARE_METATYPE(automotive::adas::HandsOnState)
Q_DECLARE_METATYPE(automotive::adas::WeatherCondition)
Q_DECLARE_METATYPE(automotive::adas::EnvironmentPreset)

#endif // AUTOMOTIVE_ADAS_TYPES_H
