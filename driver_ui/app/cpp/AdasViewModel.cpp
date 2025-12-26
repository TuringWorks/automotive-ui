// AdasViewModel.cpp
// ADAS View Model Implementation
// Safety: Provides validated data to QML - no safety logic here

#include "AdasViewModel.h"

namespace automotive {
namespace driver {

AdasViewModel::AdasViewModel(
    adas::AdasStateService* stateService,
    adas::PerceptionModel* perceptionModel,
    adas::TakeoverManager* takeoverManager,
    adas::HmiEventLog* eventLog,
    adas::AdasVisualQualityManager* qualityManager,
    QObject* parent
)
    : QObject(parent)
    , m_stateService(stateService)
    , m_perceptionModel(perceptionModel)
    , m_takeoverManager(takeoverManager)
    , m_eventLog(eventLog)
    , m_qualityManager(qualityManager)
{
    // Connect to state service
    if (m_stateService) {
        connect(m_stateService, &adas::AdasStateService::hmiStateChanged,
                this, &AdasViewModel::onHmiStateChanged);
        connect(m_stateService, &adas::AdasStateService::engagementUpdated,
                this, &AdasViewModel::onEngagementUpdated);
        connect(m_stateService, &adas::AdasStateService::availableChanged,
                this, &AdasViewModel::availableChanged);
        connect(m_stateService, &adas::AdasStateService::engagedChanged,
                this, &AdasViewModel::engagedChanged);
        connect(m_stateService, &adas::AdasStateService::degradedChanged,
                this, &AdasViewModel::degradedChanged);
    }

    // Connect to perception model
    if (m_perceptionModel) {
        connect(m_perceptionModel, &adas::PerceptionModel::laneModelChanged,
                this, &AdasViewModel::onLaneModelChanged);
        connect(m_perceptionModel, &adas::PerceptionModel::leadVehicleChanged,
                this, &AdasViewModel::onLeadVehicleChanged);
        connect(m_perceptionModel, &adas::PerceptionModel::objectListChanged,
                this, &AdasViewModel::onObjectListChanged);
    }

    // Connect to takeover manager
    if (m_takeoverManager) {
        connect(m_takeoverManager, &adas::TakeoverManager::stateChanged,
                this, &AdasViewModel::onTakeoverStateChanged);
        connect(m_takeoverManager, &adas::TakeoverManager::countdownChanged,
                this, &AdasViewModel::onTakeoverCountdownChanged);
        connect(m_takeoverManager, &adas::TakeoverManager::messageChanged,
                this, &AdasViewModel::takeoverMessageChanged);
    }

    // Connect to quality manager
    if (m_qualityManager) {
        connect(m_qualityManager, &adas::AdasVisualQualityManager::qualityChanged,
                this, &AdasViewModel::onQualityChanged);
    }
}

AdasViewModel::~AdasViewModel() = default;

// ADAS State accessors
int AdasViewModel::hmiState() const
{
    return m_stateService ? m_stateService->hmiStateInt() : 0;
}

QString AdasViewModel::hmiStateText() const
{
    return m_stateService ? m_stateService->stateText() : QString();
}

int AdasViewModel::automationLevel() const
{
    return m_stateService ? m_stateService->automationLevelInt() : 0;
}

QString AdasViewModel::automationLevelText() const
{
    if (!m_stateService) return QString();
    return adas::automationLevelToString(m_stateService->automationLevel());
}

bool AdasViewModel::isAvailable() const
{
    return m_stateService ? m_stateService->isAvailable() : false;
}

bool AdasViewModel::isEngaged() const
{
    return m_stateService ? m_stateService->isEngaged() : false;
}

bool AdasViewModel::isDegraded() const
{
    return m_stateService ? m_stateService->isDegraded() : false;
}

QString AdasViewModel::responsibilityText() const
{
    return m_stateService ? m_stateService->responsibilityText() : QString();
}

// ACC State accessors
double AdasViewModel::setSpeed() const
{
    return m_stateService ? m_stateService->setSpeed() : 0.0;
}

double AdasViewModel::followingGap() const
{
    return m_stateService ? m_stateService->followingGap() : 0.0;
}

QString AdasViewModel::featureSummary() const
{
    return m_stateService ? m_stateService->featureSummary() : QString();
}

bool AdasViewModel::isAccActive() const
{
    return m_stateService ? m_stateService->isAccActive() : false;
}

bool AdasViewModel::isLkaActive() const
{
    return m_stateService ? m_stateService->isLkaActive() : false;
}

bool AdasViewModel::isLcaActive() const
{
    return m_stateService ? m_stateService->isLcaActive() : false;
}

bool AdasViewModel::isBsmActive() const
{
    return m_stateService ? m_stateService->isBsmActive() : false;
}

// Perception accessors
bool AdasViewModel::hasValidLanes() const
{
    return m_perceptionModel ? m_perceptionModel->hasValidLanes() : false;
}

int AdasViewModel::laneQuality() const
{
    return m_perceptionModel ? m_perceptionModel->laneQualityInt() : 0;
}

double AdasViewModel::lateralOffset() const
{
    return m_perceptionModel ? m_perceptionModel->lateralOffset() : 0.0;
}

bool AdasViewModel::hasLeadVehicle() const
{
    return m_perceptionModel ? m_perceptionModel->hasLeadVehicle() : false;
}

double AdasViewModel::leadDistance() const
{
    return m_perceptionModel ? m_perceptionModel->leadDistance() : 0.0;
}

double AdasViewModel::leadRelativeSpeed() const
{
    return m_perceptionModel ? m_perceptionModel->leadRelativeSpeed() : 0.0;
}

double AdasViewModel::timeGap() const
{
    return m_perceptionModel ? m_perceptionModel->timeGap() : 0.0;
}

bool AdasViewModel::leadIsClosing() const
{
    if (!m_perceptionModel) return false;
    return m_perceptionModel->leadRelativeSpeed() < -0.5;
}

int AdasViewModel::objectCount() const
{
    return m_perceptionModel ? m_perceptionModel->objectCount() : 0;
}

// Takeover accessors
bool AdasViewModel::isTakeoverActive() const
{
    return m_takeoverManager ? m_takeoverManager->isActive() : false;
}

int AdasViewModel::takeoverUrgency() const
{
    return m_takeoverManager ? m_takeoverManager->urgencyInt() : 0;
}

double AdasViewModel::takeoverCountdown() const
{
    return m_takeoverManager ? m_takeoverManager->countdown() : 0.0;
}

QString AdasViewModel::takeoverMessage() const
{
    return m_takeoverManager ? m_takeoverManager->message() : QString();
}

bool AdasViewModel::isMrmActive() const
{
    return m_takeoverManager ? m_takeoverManager->isMrmActive() : false;
}

bool AdasViewModel::requiresImmediateAction() const
{
    return m_takeoverManager ? m_takeoverManager->requiresImmediateAction() : false;
}

// DMS accessors
bool AdasViewModel::isDriverAttentive() const
{
    if (!m_stateService) return true;
    return m_stateService->dmsStatus().isAttentive();
}

bool AdasViewModel::handsOnWheel() const
{
    if (!m_stateService) return true;
    return m_stateService->dmsStatus().handsOnState == adas::HandsOnState::HandsOn;
}

bool AdasViewModel::showHandsOnReminder() const
{
    if (!m_stateService) return false;
    return m_stateService->dmsStatus().needsHandsOnReminder();
}

// Quality accessors
int AdasViewModel::qualityLevel() const
{
    return m_qualityManager ? m_qualityManager->qualityLevelInt() : 1;
}

int AdasViewModel::environmentPreset() const
{
    return m_environmentPreset;
}

void AdasViewModel::setEnvironmentPreset(int preset)
{
    if (m_environmentPreset != preset) {
        m_environmentPreset = preset;
        emit environmentChanged();
    }
}

int AdasViewModel::weatherCondition() const
{
    return m_weatherCondition;
}

void AdasViewModel::setWeatherCondition(int condition)
{
    if (m_weatherCondition != condition) {
        m_weatherCondition = condition;
        emit weatherChanged();
    }
}

QStringList AdasViewModel::limitations() const
{
    if (!m_stateService) return {};
    return m_stateService->engagement().limitations;
}

// Data for 3D scene
QVariantMap AdasViewModel::getLaneModelData() const
{
    QVariantMap data;
    if (!m_perceptionModel) return data;

    auto lanes = m_perceptionModel->laneModel();
    data[QStringLiteral("valid")] = lanes.hasValidLanes();
    data[QStringLiteral("leftConfidence")] = static_cast<int>(lanes.left.confidence);
    data[QStringLiteral("rightConfidence")] = static_cast<int>(lanes.right.confidence);
    data[QStringLiteral("leftType")] = static_cast<int>(lanes.left.type);
    data[QStringLiteral("rightType")] = static_cast<int>(lanes.right.type);
    data[QStringLiteral("lateralOffset")] = lanes.lateralOffsetM;
    data[QStringLiteral("laneWidth")] = lanes.laneWidthM;

    return data;
}

QVariantMap AdasViewModel::getLeadVehicleData() const
{
    QVariantMap data;
    if (!m_perceptionModel) return data;

    auto lead = m_perceptionModel->leadVehicle();
    data[QStringLiteral("detected")] = lead.detected;
    data[QStringLiteral("distanceM")] = lead.distanceM;
    data[QStringLiteral("lateralOffsetM")] = lead.lateralOffsetM;
    data[QStringLiteral("relativeSpeedMps")] = lead.relativeSpeedMps;
    data[QStringLiteral("timeGapSec")] = lead.timeGapSec;
    data[QStringLiteral("isClosing")] = lead.isClosing();

    return data;
}

QVariantList AdasViewModel::getObjectListData() const
{
    QVariantList list;
    if (!m_perceptionModel) return list;

    auto objects = m_perceptionModel->objectList();
    for (const auto& obj : objects.objects) {
        QVariantMap item;
        item[QStringLiteral("id")] = obj.id;
        item[QStringLiteral("type")] = static_cast<int>(obj.type);
        item[QStringLiteral("x")] = obj.position.x;
        item[QStringLiteral("y")] = obj.position.y;
        item[QStringLiteral("heading")] = obj.heading;
        item[QStringLiteral("isRelevant")] = obj.isRelevant;
        item[QStringLiteral("isThreat")] = obj.isThreat;
        list.append(item);
    }

    return list;
}

// Slots
void AdasViewModel::onHmiStateChanged()
{
    emit hmiStateChanged();
    emit responsibilityChanged();
}

void AdasViewModel::onEngagementUpdated()
{
    emit setSpeedChanged();
    emit followingGapChanged();
    emit featuresChanged();
    emit limitationsChanged();
    emit automationLevelChanged();
}

void AdasViewModel::onLaneModelChanged()
{
    emit laneModelChanged();
}

void AdasViewModel::onLeadVehicleChanged()
{
    emit leadVehicleChanged();
}

void AdasViewModel::onObjectListChanged()
{
    emit objectListChanged();
}

void AdasViewModel::onTakeoverStateChanged()
{
    emit takeoverStateChanged();
    if (m_takeoverManager && m_takeoverManager->isMrmActive()) {
        emit mrmActiveChanged();
    }
}

void AdasViewModel::onTakeoverCountdownChanged()
{
    emit takeoverCountdownChanged();
}

void AdasViewModel::onDmsUpdated()
{
    emit dmsChanged();
}

void AdasViewModel::onQualityChanged()
{
    emit qualityChanged();
}

} // namespace driver
} // namespace automotive
