// AdasViewModel.h
// ADAS View Model for Driver UI
// Part of: Driver UI Application Layer
// Safety: Bridge between C++ ADAS services and QML rendering
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4

#ifndef AUTOMOTIVE_ADAS_VIEW_MODEL_H
#define AUTOMOTIVE_ADAS_VIEW_MODEL_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "adas/AdasStateService.h"
#include "adas/PerceptionModel.h"
#include "adas/TakeoverManager.h"
#include "adas/HmiEventLog.h"
#include "adas/AdasVisualQualityManager.h"

namespace automotive {
namespace driver {

/**
 * @brief ADAS View Model for QML
 *
 * Provides validated, formatted ADAS data to QML.
 * All safety logic remains in the underlying C++ services.
 *
 * Safety: QML receives only display-ready data.
 * No safety decisions in QML.
 */
class AdasViewModel : public QObject {
    Q_OBJECT

    // ADAS State
    Q_PROPERTY(int hmiState READ hmiState NOTIFY hmiStateChanged)
    Q_PROPERTY(QString hmiStateText READ hmiStateText NOTIFY hmiStateChanged)
    Q_PROPERTY(int automationLevel READ automationLevel NOTIFY automationLevelChanged)
    Q_PROPERTY(QString automationLevelText READ automationLevelText NOTIFY automationLevelChanged)
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
    Q_PROPERTY(bool engaged READ isEngaged NOTIFY engagedChanged)
    Q_PROPERTY(bool degraded READ isDegraded NOTIFY degradedChanged)
    Q_PROPERTY(QString responsibilityText READ responsibilityText NOTIFY responsibilityChanged)

    // ACC State
    Q_PROPERTY(double setSpeed READ setSpeed NOTIFY setSpeedChanged)
    Q_PROPERTY(double followingGap READ followingGap NOTIFY followingGapChanged)
    Q_PROPERTY(QString featureSummary READ featureSummary NOTIFY featuresChanged)
    Q_PROPERTY(bool accActive READ isAccActive NOTIFY featuresChanged)
    Q_PROPERTY(bool lkaActive READ isLkaActive NOTIFY featuresChanged)
    Q_PROPERTY(bool lcaActive READ isLcaActive NOTIFY featuresChanged)
    Q_PROPERTY(bool bsmActive READ isBsmActive NOTIFY featuresChanged)

    // Perception
    Q_PROPERTY(bool hasValidLanes READ hasValidLanes NOTIFY laneModelChanged)
    Q_PROPERTY(int laneQuality READ laneQuality NOTIFY laneModelChanged)
    Q_PROPERTY(double lateralOffset READ lateralOffset NOTIFY laneModelChanged)
    Q_PROPERTY(bool hasLeadVehicle READ hasLeadVehicle NOTIFY leadVehicleChanged)
    Q_PROPERTY(double leadDistance READ leadDistance NOTIFY leadVehicleChanged)
    Q_PROPERTY(double leadRelativeSpeed READ leadRelativeSpeed NOTIFY leadVehicleChanged)
    Q_PROPERTY(double timeGap READ timeGap NOTIFY leadVehicleChanged)
    Q_PROPERTY(bool leadIsClosing READ leadIsClosing NOTIFY leadVehicleChanged)
    Q_PROPERTY(int objectCount READ objectCount NOTIFY objectListChanged)

    // Takeover
    Q_PROPERTY(bool takeoverActive READ isTakeoverActive NOTIFY takeoverStateChanged)
    Q_PROPERTY(int takeoverUrgency READ takeoverUrgency NOTIFY takeoverStateChanged)
    Q_PROPERTY(double takeoverCountdown READ takeoverCountdown NOTIFY takeoverCountdownChanged)
    Q_PROPERTY(QString takeoverMessage READ takeoverMessage NOTIFY takeoverMessageChanged)
    Q_PROPERTY(bool mrmActive READ isMrmActive NOTIFY mrmActiveChanged)
    Q_PROPERTY(bool requiresImmediateAction READ requiresImmediateAction NOTIFY takeoverStateChanged)

    // DMS
    Q_PROPERTY(bool driverAttentive READ isDriverAttentive NOTIFY dmsChanged)
    Q_PROPERTY(bool handsOnWheel READ handsOnWheel NOTIFY dmsChanged)
    Q_PROPERTY(bool showHandsOnReminder READ showHandsOnReminder NOTIFY dmsChanged)

    // Quality
    Q_PROPERTY(int qualityLevel READ qualityLevel NOTIFY qualityChanged)
    Q_PROPERTY(int environmentPreset READ environmentPreset
               WRITE setEnvironmentPreset NOTIFY environmentChanged)
    Q_PROPERTY(int weatherCondition READ weatherCondition
               WRITE setWeatherCondition NOTIFY weatherChanged)

    // Limitations
    Q_PROPERTY(QStringList limitations READ limitations NOTIFY limitationsChanged)

public:
    explicit AdasViewModel(
        adas::AdasStateService* stateService,
        adas::PerceptionModel* perceptionModel,
        adas::TakeoverManager* takeoverManager,
        adas::HmiEventLog* eventLog,
        adas::AdasVisualQualityManager* qualityManager,
        QObject* parent = nullptr
    );
    ~AdasViewModel() override;

    // ADAS State
    int hmiState() const;
    QString hmiStateText() const;
    int automationLevel() const;
    QString automationLevelText() const;
    bool isAvailable() const;
    bool isEngaged() const;
    bool isDegraded() const;
    QString responsibilityText() const;

    // ACC State
    double setSpeed() const;
    double followingGap() const;
    QString featureSummary() const;
    bool isAccActive() const;
    bool isLkaActive() const;
    bool isLcaActive() const;
    bool isBsmActive() const;

    // Perception
    bool hasValidLanes() const;
    int laneQuality() const;
    double lateralOffset() const;
    bool hasLeadVehicle() const;
    double leadDistance() const;
    double leadRelativeSpeed() const;
    double timeGap() const;
    bool leadIsClosing() const;
    int objectCount() const;

    // Takeover
    bool isTakeoverActive() const;
    int takeoverUrgency() const;
    double takeoverCountdown() const;
    QString takeoverMessage() const;
    bool isMrmActive() const;
    bool requiresImmediateAction() const;

    // DMS
    bool isDriverAttentive() const;
    bool handsOnWheel() const;
    bool showHandsOnReminder() const;

    // Quality
    int qualityLevel() const;
    int environmentPreset() const;
    void setEnvironmentPreset(int preset);
    int weatherCondition() const;
    void setWeatherCondition(int condition);

    // Limitations
    QStringList limitations() const;

    // Data for 3D scene (QVariantMap for QML)
    Q_INVOKABLE QVariantMap getLaneModelData() const;
    Q_INVOKABLE QVariantMap getLeadVehicleData() const;
    Q_INVOKABLE QVariantList getObjectListData() const;

signals:
    // State signals
    void hmiStateChanged();
    void automationLevelChanged();
    void availableChanged();
    void engagedChanged();
    void degradedChanged();
    void responsibilityChanged();

    // ACC signals
    void setSpeedChanged();
    void followingGapChanged();
    void featuresChanged();

    // Perception signals
    void laneModelChanged();
    void leadVehicleChanged();
    void objectListChanged();

    // Takeover signals
    void takeoverStateChanged();
    void takeoverCountdownChanged();
    void takeoverMessageChanged();
    void mrmActiveChanged();

    // DMS signals
    void dmsChanged();

    // Quality signals
    void qualityChanged();
    void environmentChanged();
    void weatherChanged();

    // Limitations
    void limitationsChanged();

private slots:
    void onHmiStateChanged();
    void onEngagementUpdated();
    void onLaneModelChanged();
    void onLeadVehicleChanged();
    void onObjectListChanged();
    void onTakeoverStateChanged();
    void onTakeoverCountdownChanged();
    void onDmsUpdated();
    void onQualityChanged();

private:
    adas::AdasStateService* m_stateService{nullptr};
    adas::PerceptionModel* m_perceptionModel{nullptr};
    adas::TakeoverManager* m_takeoverManager{nullptr};
    adas::HmiEventLog* m_eventLog{nullptr};
    adas::AdasVisualQualityManager* m_qualityManager{nullptr};

    // Local state for visualization (not safety-critical)
    int m_environmentPreset{1};  // Day
    int m_weatherCondition{0};   // Clear
};

} // namespace driver
} // namespace automotive

#endif // AUTOMOTIVE_ADAS_VIEW_MODEL_H
