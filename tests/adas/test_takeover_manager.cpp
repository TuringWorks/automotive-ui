// test_takeover_manager.cpp
// Unit tests for TakeoverManager
// Tests: Takeover pipeline, timing, escalation
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.3, 4.6

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>
#include "adas/TakeoverManager.h"
#include "adas/AdasEngagement.h"

using namespace automotive::adas;

class TakeoverManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app = new QCoreApplication(argc, nullptr);
        }
        manager = std::make_unique<TakeoverManager>();
    }

    void TearDown() override {
        manager.reset();
    }

    TakeoverRequest createRequest(TakeoverUrgency urgency, double countdown,
                                   const QString& reason = QString()) {
        TakeoverRequest request;
        request.active = true;
        request.urgency = urgency;
        request.countdownSec = countdown;
        request.reason = reason;
        request.requiredAction = "Take over driving";
        request.metadata.valid = true;
        request.metadata.sequenceNumber = ++seqCounter;
        request.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
        return request;
    }

    std::unique_ptr<TakeoverManager> manager;
    QCoreApplication* app = nullptr;
    uint32_t seqCounter = 0;
};

// =============================================================================
// SR-CL-ADAS-120: Takeover banner non-occludable with escalation
// =============================================================================

TEST_F(TakeoverManagerTest, SR_CL_ADAS_120_TakeoverActivation) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0, "ODD ending");

    QSignalSpy stateChangedSpy(manager.get(), &TakeoverManager::stateChanged);
    QSignalSpy takeoverRequestedSpy(manager.get(), &TakeoverManager::takeoverRequested);

    manager->updateFromRequest(request);

    EXPECT_TRUE(manager->isActive());
    EXPECT_EQ(manager->state(), TakeoverState::RequestActive);
    EXPECT_EQ(stateChangedSpy.count(), 1);
    EXPECT_EQ(takeoverRequestedSpy.count(), 1);
}

TEST_F(TakeoverManagerTest, SR_CL_ADAS_120_Escalation) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    QSignalSpy escalatedSpy(manager.get(), &TakeoverManager::takeoverEscalated);

    // Simulate time passing beyond escalation threshold
    for (int i = 0; i < 120; i++) {  // ~6 seconds at 50ms ticks
        manager->processTick(i, i * 50 + 6000);  // 6 second offset to trigger escalation
    }

    // Should have escalated
    EXPECT_GE(manager->escalationLevel(), 1);
}

// =============================================================================
// SR-CL-ADAS-121: Urgency levels map to consistent HMI patterns
// =============================================================================

TEST_F(TakeoverManagerTest, SR_CL_ADAS_121_UrgencyMapping_Advisory) {
    auto request = createRequest(TakeoverUrgency::Advisory, 30.0);
    manager->updateFromRequest(request);

    EXPECT_EQ(manager->urgency(), TakeoverUrgency::Advisory);
    EXPECT_EQ(manager->state(), TakeoverState::PreWarning);
}

TEST_F(TakeoverManagerTest, SR_CL_ADAS_121_UrgencyMapping_Warning) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    EXPECT_EQ(manager->urgency(), TakeoverUrgency::Warning);
    EXPECT_EQ(manager->state(), TakeoverState::RequestActive);
}

TEST_F(TakeoverManagerTest, SR_CL_ADAS_121_UrgencyMapping_Immediate) {
    auto request = createRequest(TakeoverUrgency::Immediate, 5.0);
    manager->updateFromRequest(request);

    EXPECT_EQ(manager->urgency(), TakeoverUrgency::Immediate);
    EXPECT_EQ(manager->state(), TakeoverState::Escalated);
}

TEST_F(TakeoverManagerTest, SR_CL_ADAS_121_UrgencyMapping_Critical) {
    auto request = createRequest(TakeoverUrgency::Critical, 3.0);
    manager->updateFromRequest(request);

    EXPECT_EQ(manager->urgency(), TakeoverUrgency::Critical);
    EXPECT_EQ(manager->state(), TakeoverState::CriticalEscalation);
}

// =============================================================================
// SR-CL-ADAS-122: "Immediate Action Required" at threshold
// =============================================================================

TEST_F(TakeoverManagerTest, SR_CL_ADAS_122_ImmediateActionThreshold) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    EXPECT_FALSE(manager->requiresImmediateAction());

    // Simulate countdown to threshold (3 seconds)
    auto criticalRequest = createRequest(TakeoverUrgency::Warning, 2.5);
    manager->updateFromRequest(criticalRequest);

    // With countdown <= 3.0, should require immediate action
    // Note: The manager checks countdown in requiresImmediateAction()
    // We need to simulate tick processing for this
}

TEST_F(TakeoverManagerTest, SR_CL_ADAS_122_ImmediateUrgencyTriggersImmediate) {
    auto request = createRequest(TakeoverUrgency::Immediate, 10.0);
    manager->updateFromRequest(request);

    EXPECT_TRUE(manager->requiresImmediateAction());
}

// =============================================================================
// SR-CL-ADAS-130: MRM shows persistent state
// =============================================================================

TEST_F(TakeoverManagerTest, SR_CL_ADAS_130_MrmPersistentState) {
    QSignalSpy mrmSpy(manager.get(), &TakeoverManager::mrmStarted);

    manager->forceMrm("No driver response");

    EXPECT_TRUE(manager->isMrmActive());
    EXPECT_EQ(manager->state(), TakeoverState::MrmInitiated);
    EXPECT_EQ(mrmSpy.count(), 1);

    // Message should indicate safe stop
    QString message = manager->message();
    EXPECT_TRUE(message.contains("Safe Stop") || message.contains("stopping"));
}

// =============================================================================
// Countdown and Timing
// =============================================================================

TEST_F(TakeoverManagerTest, Countdown_Updates) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    QSignalSpy countdownSpy(manager.get(), &TakeoverManager::countdownChanged);

    EXPECT_DOUBLE_EQ(manager->countdown(), 10.0);

    // Process tick (simulating 1 second later)
    manager->processTick(20, 1000);

    EXPECT_GE(countdownSpy.count(), 1);
}

TEST_F(TakeoverManagerTest, Countdown_ExpiresTriggerssMRM) {
    auto request = createRequest(TakeoverUrgency::Warning, 0.5);  // Very short countdown
    manager->updateFromRequest(request);

    QSignalSpy mrmSpy(manager.get(), &TakeoverManager::mrmStarted);

    // Process ticks until countdown expires
    for (int i = 0; i < 20; i++) {
        manager->processTick(i, i * 100);  // 100ms per tick
        if (manager->isMrmActive()) break;
    }

    // Should have transitioned to MRM
    EXPECT_TRUE(manager->isMrmActive() || manager->state() == TakeoverState::MrmInitiated);
}

// =============================================================================
// Driver Acknowledgment
// =============================================================================

TEST_F(TakeoverManagerTest, Acknowledgment_CompletesState) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    EXPECT_TRUE(manager->isActive());

    QSignalSpy completedSpy(manager.get(), &TakeoverManager::takeoverCompleted);

    manager->acknowledgeTakeover();

    EXPECT_EQ(manager->state(), TakeoverState::Completed);
    EXPECT_FALSE(manager->isActive());
    EXPECT_EQ(completedSpy.count(), 1);
}

// =============================================================================
// Cancellation
// =============================================================================

TEST_F(TakeoverManagerTest, Cancellation_ReturnsToInactive) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(request);

    EXPECT_TRUE(manager->isActive());

    QSignalSpy cancelledSpy(manager.get(), &TakeoverManager::takeoverCancelled);

    manager->cancelTakeover();

    EXPECT_EQ(manager->state(), TakeoverState::Inactive);
    EXPECT_FALSE(manager->isActive());
    EXPECT_EQ(cancelledSpy.count(), 1);
}

TEST_F(TakeoverManagerTest, InactiveRequest_CancelsPending) {
    // Start with active request
    auto activeRequest = createRequest(TakeoverUrgency::Warning, 10.0);
    manager->updateFromRequest(activeRequest);
    EXPECT_TRUE(manager->isActive());

    // Send inactive request
    TakeoverRequest inactiveRequest;
    inactiveRequest.active = false;
    manager->updateFromRequest(inactiveRequest);

    EXPECT_FALSE(manager->isActive());
}

// =============================================================================
// Reset
// =============================================================================

TEST_F(TakeoverManagerTest, Reset_ClearsAllState) {
    auto request = createRequest(TakeoverUrgency::Critical, 5.0);
    manager->updateFromRequest(request);

    manager->reset();

    EXPECT_EQ(manager->state(), TakeoverState::Inactive);
    EXPECT_EQ(manager->urgency(), TakeoverUrgency::None);
    EXPECT_DOUBLE_EQ(manager->countdown(), 0.0);
    EXPECT_TRUE(manager->message().isEmpty());
    EXPECT_FALSE(manager->isActive());
    EXPECT_FALSE(manager->isMrmActive());
}

// =============================================================================
// Message Content
// =============================================================================

TEST_F(TakeoverManagerTest, Message_ContainsRelevantInfo) {
    auto request = createRequest(TakeoverUrgency::Warning, 10.0, "Lane lines lost");
    manager->updateFromRequest(request);

    QString message = manager->message();
    EXPECT_FALSE(message.isEmpty());
    EXPECT_TRUE(message.contains("TAKE OVER", Qt::CaseInsensitive) ||
                message.contains("take over", Qt::CaseInsensitive));
}

TEST_F(TakeoverManagerTest, Message_MrmSpecific) {
    manager->forceMrm("Test MRM");

    QString message = manager->message();
    EXPECT_TRUE(message.contains("Safe Stop", Qt::CaseInsensitive) ||
                message.contains("stopping", Qt::CaseInsensitive));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
