// AdasEngagement.h
// ADAS Engagement and Availability Model
// Part of: Shared ADAS Platform Layer
// Safety: Core data contract for ADAS state presentation
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.2

#ifndef AUTOMOTIVE_ADAS_ENGAGEMENT_H
#define AUTOMOTIVE_ADAS_ENGAGEMENT_H

#include "AdasTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>

namespace automotive {
namespace adas {

/**
 * @brief ADAS Engagement Data Contract
 *
 * Core topic: AdasEngagement
 * Requirement: ADAS_Autonomy_UI_Spec Section 3.2
 *
 * All signals include: timestamp, validity, confidence, source, seq, units.
 */
struct AdasEngagement {
    // Metadata
    SignalMetadata metadata;

    // Core state
    AutomationLevel level{AutomationLevel::L0_NoAutomation};
    AdasHmiState hmiState{AdasHmiState::Off};
    bool available{false};          ///< System is ready to engage
    bool engaged{false};            ///< System is currently active

    // ACC settings
    double setSpeedKph{0.0};        ///< Set speed in km/h
    double followingGapSeconds{2.0}; ///< Following distance in seconds

    // Active features
    bool accActive{false};          ///< Adaptive Cruise Control
    bool lkaActive{false};          ///< Lane Keeping Assist
    bool lcaActive{false};          ///< Lane Centering Assist
    bool bsmActive{false};          ///< Blind Spot Monitoring
    bool aebArmed{false};           ///< Automatic Emergency Braking armed

    // Limits and constraints
    double maxSpeedKph{200.0};      ///< Maximum allowed set speed
    double minSpeedKph{30.0};       ///< Minimum allowed set speed
    double minFollowingGap{0.8};    ///< Minimum following gap
    double maxFollowingGap{3.5};    ///< Maximum following gap

    // Limitation reasons (for UI)
    QStringList limitations;         ///< Current limitation messages

    /**
     * @brief Check if engagement can be displayed as "active"
     *
     * Requirement: SR-CL-ADAS-100
     * Never show ENGAGED unless engaged=true AND available=true AND valid
     */
    bool canShowEngaged() const {
        return engaged && available && metadata.isValid();
    }

    /**
     * @brief Get active feature count
     */
    int activeFeatureCount() const {
        int count = 0;
        if (accActive) ++count;
        if (lkaActive) ++count;
        if (lcaActive) ++count;
        if (bsmActive) ++count;
        return count;
    }

    /**
     * @brief Get feature summary string
     */
    QString featureSummary() const {
        QStringList features;
        if (accActive) features << QStringLiteral("ACC");
        if (lkaActive) features << QStringLiteral("LKA");
        if (lcaActive) features << QStringLiteral("LCA");
        if (bsmActive) features << QStringLiteral("BSM");
        return features.join(QStringLiteral(" + "));
    }
};

/**
 * @brief ODD Status Data Contract
 *
 * Core topic: OddStatus
 * Defines the Operational Design Domain state.
 */
struct OddData {
    SignalMetadata metadata;

    OddStatus status{OddStatus::Unknown};

    // ODD boundaries
    QString roadType;               ///< "highway", "urban", "rural"
    double maxSpeedKph{0.0};        ///< Maximum speed for current ODD
    double minSpeedKph{0.0};        ///< Minimum speed for current ODD
    WeatherCondition weatherLimit{WeatherCondition::Clear};
    bool geoFenceActive{false};     ///< Geofence restriction active

    // Boundary proximity (for pre-warnings)
    double distanceToBoundaryM{-1.0}; ///< Distance to ODD boundary (-1 = unknown)
    double timeToBoundarySec{-1.0};   ///< Time to ODD boundary (-1 = unknown)

    // Reasons for ODD status
    QStringList oddReasons;          ///< Why inside/outside ODD

    /**
     * @brief Check if approaching ODD boundary
     */
    bool isApproachingBoundary(double thresholdSec = 30.0) const {
        return timeToBoundarySec > 0 && timeToBoundarySec <= thresholdSec;
    }
};

/**
 * @brief Takeover Request Data Contract
 *
 * Core topic: TakeoverRequest
 * Requirement: SR-CL-ADAS-120
 */
struct TakeoverRequest {
    SignalMetadata metadata;

    bool active{false};             ///< Takeover request is active
    TakeoverUrgency urgency{TakeoverUrgency::None};
    double countdownSec{0.0};       ///< Remaining time for takeover
    QString requiredAction;         ///< What the driver should do
    QString reason;                 ///< Why takeover is requested

    // Escalation tracking
    int escalationLevel{0};         ///< How many times escalated
    bool audioActive{false};        ///< Audio alert is playing
    bool hapticActive{false};       ///< Haptic feedback is active

    /**
     * @brief Check if immediate action is required
     *
     * Requirement: SR-CL-ADAS-122
     */
    bool requiresImmediateAction() const {
        return active && (urgency >= TakeoverUrgency::Immediate || countdownSec <= 5.0);
    }
};

/**
 * @brief Driver Monitoring Status Data Contract
 *
 * Core topic: DmsStatus
 */
struct DmsStatus {
    SignalMetadata metadata;

    HandsOnState handsOnState{HandsOnState::Unknown};
    AttentionState attentionState{AttentionState::Unknown};
    bool distractionDetected{false};

    // Gaze tracking summary (raw data not exposed to UI)
    bool eyesOnRoad{true};
    double gazeDurationOffRoadSec{0.0};

    // Reminder state
    bool handsOnReminderActive{false};
    bool attentionReminderActive{false};
    int reminderCount{0};           ///< How many reminders issued

    /**
     * @brief Check if driver is considered attentive
     */
    bool isAttentive() const {
        return attentionState == AttentionState::Attentive &&
               !distractionDetected &&
               eyesOnRoad;
    }

    /**
     * @brief Check if hands-on reminder should be shown
     */
    bool needsHandsOnReminder() const {
        return handsOnState == HandsOnState::HandsOff && handsOnReminderActive;
    }
};

} // namespace adas
} // namespace automotive

Q_DECLARE_METATYPE(automotive::adas::AdasEngagement)
Q_DECLARE_METATYPE(automotive::adas::OddData)
Q_DECLARE_METATYPE(automotive::adas::TakeoverRequest)
Q_DECLARE_METATYPE(automotive::adas::DmsStatus)

#endif // AUTOMOTIVE_ADAS_ENGAGEMENT_H
