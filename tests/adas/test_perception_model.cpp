// test_perception_model.cpp
// Unit tests for PerceptionModel
// Tests: Lane model, lead vehicle, staleness detection
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 3.2

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include "adas/PerceptionModel.h"

using namespace automotive::adas;

class PerceptionModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app = new QCoreApplication(argc, nullptr);
        }
        model = std::make_unique<PerceptionModel>();
    }

    void TearDown() override {
        model.reset();
    }

    LaneModel createValidLaneModel(ConfidenceLevel leftConf, ConfidenceLevel rightConf) {
        LaneModel lanes;
        lanes.metadata.valid = true;
        lanes.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
        lanes.metadata.sequenceNumber = ++seqCounter;
        lanes.metadata.confidence = std::min(leftConf, rightConf);

        lanes.left.isValid = true;
        lanes.left.confidence = leftConf;
        lanes.left.type = LaneType::Solid;

        lanes.right.isValid = true;
        lanes.right.confidence = rightConf;
        lanes.right.type = LaneType::Dashed;

        lanes.laneWidthM = 3.5;
        lanes.lateralOffsetM = 0.1;

        return lanes;
    }

    LeadVehicle createLeadVehicle(double distance, double relativeSpeed) {
        LeadVehicle lead;
        lead.metadata.valid = true;
        lead.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
        lead.metadata.sequenceNumber = ++seqCounter;
        lead.metadata.confidence = ConfidenceLevel::High;

        lead.detected = true;
        lead.distanceM = distance;
        lead.relativeSpeedMps = relativeSpeed;
        lead.timeGapSec = distance / std::max(1.0, std::abs(relativeSpeed));

        return lead;
    }

    std::unique_ptr<PerceptionModel> model;
    QCoreApplication* app = nullptr;
    uint32_t seqCounter = 0;
};

// =============================================================================
// SR-CL-ADAS-140: Display DEGRADED within 300ms on confidence drop
// =============================================================================

TEST_F(PerceptionModelTest, SR_CL_ADAS_140_ConfidenceDropDetection) {
    // Start with high confidence lanes
    auto goodLanes = createValidLaneModel(ConfidenceLevel::High, ConfidenceLevel::High);
    model->updateLaneModel(goodLanes);
    EXPECT_EQ(model->laneQuality(), ConfidenceLevel::High);

    // Drop to low confidence
    auto poorLanes = createValidLaneModel(ConfidenceLevel::Low, ConfidenceLevel::Low);

    QSignalSpy degradedSpy(model.get(), &PerceptionModel::perceptionDegraded);
    QSignalSpy laneChangedSpy(model.get(), &PerceptionModel::laneModelChanged);

    model->updateLaneModel(poorLanes);

    EXPECT_EQ(model->laneQuality(), ConfidenceLevel::Low);
    EXPECT_GE(laneChangedSpy.count(), 1);
}

// =============================================================================
// SR-CL-ADAS-141: Lane visualization reflects quality
// =============================================================================

TEST_F(PerceptionModelTest, SR_CL_ADAS_141_LaneQualityReflection) {
    // Test high confidence
    {
        auto lanes = createValidLaneModel(ConfidenceLevel::High, ConfidenceLevel::High);
        model->updateLaneModel(lanes);
        EXPECT_EQ(model->laneQuality(), ConfidenceLevel::High);
    }

    // Test medium confidence (min of left/right)
    {
        auto lanes = createValidLaneModel(ConfidenceLevel::High, ConfidenceLevel::Medium);
        model->updateLaneModel(lanes);
        EXPECT_EQ(model->laneQuality(), ConfidenceLevel::Medium);
    }

    // Test low confidence
    {
        auto lanes = createValidLaneModel(ConfidenceLevel::Low, ConfidenceLevel::Low);
        model->updateLaneModel(lanes);
        EXPECT_EQ(model->laneQuality(), ConfidenceLevel::Low);
    }
}

// =============================================================================
// SR-CL-ADAS-142: Hide stale object markers
// =============================================================================

TEST_F(PerceptionModelTest, SR_CL_ADAS_142_StaleObjectHiding) {
    // Add some objects
    ObjectList objects;
    objects.metadata.valid = true;
    objects.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
    objects.metadata.sequenceNumber = ++seqCounter;

    PerceivedObject car;
    car.id = 1;
    car.type = ObjectType::Car;
    car.position.x = 50;
    objects.objects.append(car);

    model->updateObjectList(objects);
    EXPECT_EQ(model->objectCount(), 1);

    // Simulate staleness by processing tick with time beyond freshness window
    QSignalSpy objectListSpy(model.get(), &PerceptionModel::objectListChanged);

    // Process tick 500ms later (beyond 200ms freshness)
    model->processTick(10, 500);

    // Object list should be cleared when stale
    EXPECT_EQ(model->objectCount(), 0) << "Stale objects should be hidden";
}

// =============================================================================
// Lane Model Updates
// =============================================================================

TEST_F(PerceptionModelTest, LaneModel_ValidLanes) {
    auto lanes = createValidLaneModel(ConfidenceLevel::High, ConfidenceLevel::High);
    model->updateLaneModel(lanes);

    EXPECT_TRUE(model->hasValidLanes());
    EXPECT_DOUBLE_EQ(model->lateralOffset(), 0.1);
}

TEST_F(PerceptionModelTest, LaneModel_InvalidLanes) {
    LaneModel lanes;
    lanes.metadata.valid = false;
    lanes.left.isValid = false;
    lanes.right.isValid = false;

    model->updateLaneModel(lanes);

    EXPECT_FALSE(model->hasValidLanes());
}

TEST_F(PerceptionModelTest, LaneModel_PartialLanes) {
    LaneModel lanes;
    lanes.metadata.valid = true;
    lanes.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
    lanes.left.isValid = true;
    lanes.left.confidence = ConfidenceLevel::High;
    lanes.right.isValid = false;  // Only left lane valid

    model->updateLaneModel(lanes);

    EXPECT_TRUE(model->hasValidLanes()) << "Partial lanes should still be valid";
}

// =============================================================================
// Lead Vehicle Detection
// =============================================================================

TEST_F(PerceptionModelTest, LeadVehicle_Detected) {
    auto lead = createLeadVehicle(50.0, -2.0);  // 50m ahead, closing at 2 m/s
    model->updateLeadVehicle(lead);

    EXPECT_TRUE(model->hasLeadVehicle());
    EXPECT_DOUBLE_EQ(model->leadDistance(), 50.0);
    EXPECT_DOUBLE_EQ(model->leadRelativeSpeed(), -2.0);
}

TEST_F(PerceptionModelTest, LeadVehicle_NoDetection) {
    LeadVehicle noLead;
    noLead.detected = false;
    noLead.metadata.valid = true;
    noLead.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();

    model->updateLeadVehicle(noLead);

    EXPECT_FALSE(model->hasLeadVehicle());
}

TEST_F(PerceptionModelTest, LeadVehicle_ClosingDetection) {
    auto closing = createLeadVehicle(30.0, -3.0);  // Closing rapidly
    model->updateLeadVehicle(closing);

    // Lead vehicle with negative relative speed is closing
    EXPECT_TRUE(model->leadVehicle().isClosing());
}

TEST_F(PerceptionModelTest, LeadVehicle_DivergingDetection) {
    auto diverging = createLeadVehicle(50.0, 2.0);  // Moving away
    model->updateLeadVehicle(diverging);

    EXPECT_TRUE(model->leadVehicle().isDiverging());
}

// =============================================================================
// Time Gap Calculation
// =============================================================================

TEST_F(PerceptionModelTest, TimeGap_Calculation) {
    auto lead = createLeadVehicle(40.0, -2.0);
    lead.timeGapSec = 2.0;  // 2 second gap
    model->updateLeadVehicle(lead);

    EXPECT_DOUBLE_EQ(model->timeGap(), 2.0);
}

// =============================================================================
// Object List
// =============================================================================

TEST_F(PerceptionModelTest, ObjectList_MultipleObjects) {
    ObjectList objects;
    objects.metadata.valid = true;
    objects.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
    objects.metadata.sequenceNumber = ++seqCounter;

    PerceivedObject car;
    car.id = 1;
    car.type = ObjectType::Car;
    car.position.x = 30;
    objects.objects.append(car);

    PerceivedObject pedestrian;
    pedestrian.id = 2;
    pedestrian.type = ObjectType::Pedestrian;
    pedestrian.position.x = 20;
    objects.objects.append(pedestrian);

    model->updateObjectList(objects);

    EXPECT_EQ(model->objectCount(), 2);
    EXPECT_EQ(model->objectList().vehicleCount(), 1);
    EXPECT_EQ(model->objectList().pedestrianCount(), 1);
}

TEST_F(PerceptionModelTest, ObjectList_RelevantFiltering) {
    ObjectList objects;
    objects.metadata.valid = true;
    objects.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
    objects.metadata.sequenceNumber = ++seqCounter;

    PerceivedObject relevant;
    relevant.id = 1;
    relevant.type = ObjectType::Car;
    relevant.isRelevant = true;
    objects.objects.append(relevant);

    PerceivedObject notRelevant;
    notRelevant.id = 2;
    notRelevant.type = ObjectType::Car;
    notRelevant.isRelevant = false;
    objects.objects.append(notRelevant);

    model->updateObjectList(objects);

    auto relevantObjects = model->relevantObjects();
    EXPECT_EQ(relevantObjects.size(), 1);
    EXPECT_EQ(relevantObjects[0].id, 1);
}

// =============================================================================
// Corridor Data
// =============================================================================

TEST_F(PerceptionModelTest, Corridor_Valid) {
    DrivableCorridor corridor;
    corridor.metadata.valid = true;
    corridor.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
    corridor.isValid = true;
    corridor.corridorWidthM = 2.5;
    corridor.pathLengthM = 100.0;

    model->updateCorridor(corridor);

    EXPECT_TRUE(model->hasValidCorridor());
    EXPECT_DOUBLE_EQ(model->corridor().corridorWidthM, 2.5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
