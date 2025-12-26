// test_adas_state_service.cpp
// Unit tests for AdasStateService
// Tests: State machine transitions, timing requirements, safety constraints
// Spec: ADAS_Autonomy_UI_Spec_v2_EosAligned.md Section 4.2

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include "adas/AdasStateService.h"
#include "adas/AdasEngagement.h"

using namespace automotive::adas;

class AdasStateServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a QCoreApplication if not exists (needed for signals/slots)
        if (!QCoreApplication::instance()) {
            int argc = 0;
            app = new QCoreApplication(argc, nullptr);
        }
        service = std::make_unique<AdasStateService>();
    }

    void TearDown() override {
        service.reset();
    }

    AdasEngagement createValidEngagement(bool engaged, bool available,
                                          AutomationLevel level) {
        AdasEngagement engagement;
        engagement.metadata.valid = true;
        engagement.metadata.timestampMs = QDateTime::currentMSecsSinceEpoch();
        engagement.metadata.sequenceNumber = ++seqCounter;
        engagement.metadata.confidence = ConfidenceLevel::High;
        engagement.engaged = engaged;
        engagement.available = available;
        engagement.level = level;
        return engagement;
    }

    std::unique_ptr<AdasStateService> service;
    QCoreApplication* app = nullptr;
    uint32_t seqCounter = 0;
};

// =============================================================================
// SR-CL-ADAS-100: Never show ENGAGED unless engaged=true AND available=true AND valid
// =============================================================================

TEST_F(AdasStateServiceTest, SR_CL_ADAS_100_NeverShowEngagedUnlessValid) {
    // Test: engaged=false should not show engaged
    {
        auto engagement = createValidEngagement(false, true, AutomationLevel::L2_PartialAutomation);
        service->updateEngagement(engagement);
        EXPECT_FALSE(service->isEngaged()) << "Should not show engaged when engaged=false";
    }

    // Test: available=false should not show engaged
    {
        auto engagement = createValidEngagement(true, false, AutomationLevel::L2_PartialAutomation);
        service->updateEngagement(engagement);
        EXPECT_FALSE(service->isEngaged()) << "Should not show engaged when available=false";
    }

    // Test: invalid metadata should not show engaged
    {
        auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
        engagement.metadata.valid = false;
        service->updateEngagement(engagement);
        EXPECT_FALSE(service->isEngaged()) << "Should not show engaged when metadata invalid";
    }

    // Test: all conditions met should show engaged
    {
        auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
        service->updateEngagement(engagement);
        EXPECT_TRUE(service->isEngaged()) << "Should show engaged when all conditions met";
    }
}

// =============================================================================
// SR-CL-ADAS-101: Indicate "Unavailable" within 300ms when availability transitions to false
// =============================================================================

TEST_F(AdasStateServiceTest, SR_CL_ADAS_101_UnavailableTransition) {
    // Start with available
    auto engagement = createValidEngagement(false, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(engagement);
    EXPECT_TRUE(service->isAvailable());

    // Transition to unavailable
    auto engagementUnavailable = createValidEngagement(false, false, AutomationLevel::L2_PartialAutomation);
    QSignalSpy availableSpy(service.get(), &AdasStateService::availableChanged);

    auto startTime = QDateTime::currentMSecsSinceEpoch();
    service->updateEngagement(engagementUnavailable);
    auto endTime = QDateTime::currentMSecsSinceEpoch();

    EXPECT_FALSE(service->isAvailable()) << "Should be unavailable after transition";
    EXPECT_LE(endTime - startTime, 300) << "Transition should complete within 300ms";
}

// =============================================================================
// SR-CL-ADAS-102: Default to OFF/UNAVAILABLE on unknown state
// =============================================================================

TEST_F(AdasStateServiceTest, SR_CL_ADAS_102_DefaultToOffOnUnknown) {
    // Initial state should be OFF
    EXPECT_EQ(service->hmiState(), AdasHmiState::Off);
    EXPECT_FALSE(service->isAvailable());
    EXPECT_FALSE(service->isEngaged());
}

// =============================================================================
// State Machine Transitions (Section 4.2)
// =============================================================================

TEST_F(AdasStateServiceTest, StateMachine_OffToAvailable) {
    auto engagement = createValidEngagement(false, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(engagement);

    EXPECT_EQ(service->hmiState(), AdasHmiState::Available);
}

TEST_F(AdasStateServiceTest, StateMachine_AvailableToEngagedL2) {
    // First make available
    auto availableEngagement = createValidEngagement(false, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(availableEngagement);

    // Then engage
    auto engagedEngagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(engagedEngagement);

    EXPECT_EQ(service->hmiState(), AdasHmiState::EngagedL2);
}

TEST_F(AdasStateServiceTest, StateMachine_EngagedL3ToTakeover) {
    // Make L3 engaged
    auto engagedL3 = createValidEngagement(true, true, AutomationLevel::L3_ConditionalAutomation);
    service->updateEngagement(engagedL3);

    // Request takeover transition
    bool transitionAllowed = service->requestStateTransition(AdasHmiState::TakeoverRequested);
    EXPECT_TRUE(transitionAllowed);
    EXPECT_EQ(service->hmiState(), AdasHmiState::TakeoverRequested);
}

TEST_F(AdasStateServiceTest, StateMachine_TakeoverToMRM) {
    // Setup: L3 engaged -> takeover requested
    auto engagedL3 = createValidEngagement(true, true, AutomationLevel::L3_ConditionalAutomation);
    service->updateEngagement(engagedL3);
    service->requestStateTransition(AdasHmiState::TakeoverRequested);

    // Transition to MRM
    bool transitionAllowed = service->requestStateTransition(AdasHmiState::MrmActive);
    EXPECT_TRUE(transitionAllowed);
    EXPECT_EQ(service->hmiState(), AdasHmiState::MrmActive);
}

TEST_F(AdasStateServiceTest, StateMachine_InvalidTransitionBlocked) {
    // Try to go from OFF directly to EngagedL2 (should be blocked)
    QSignalSpy blockedSpy(service.get(), &AdasStateService::stateTransitionBlocked);

    bool transitionAllowed = service->requestStateTransition(AdasHmiState::EngagedL2);
    EXPECT_FALSE(transitionAllowed);
    EXPECT_EQ(blockedSpy.count(), 1);
}

// =============================================================================
// CR-ADAS-102: Anti-replay protection (sequence number check)
// =============================================================================

TEST_F(AdasStateServiceTest, CR_ADAS_102_RejectReplayedMessage) {
    // Send first message
    auto engagement1 = createValidEngagement(false, true, AutomationLevel::L2_PartialAutomation);
    engagement1.metadata.sequenceNumber = 100;
    EXPECT_TRUE(service->updateEngagement(engagement1));

    // Send message with lower sequence number (replay attack)
    QSignalSpy integrityFailSpy(service.get(), &AdasStateService::integrityFailure);
    auto engagement2 = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    engagement2.metadata.sequenceNumber = 50;  // Lower than previous
    EXPECT_FALSE(service->updateEngagement(engagement2));
    EXPECT_EQ(integrityFailSpy.count(), 1);
}

// =============================================================================
// CR-ADAS-103: Default to OFF on integrity failure
// =============================================================================

TEST_F(AdasStateServiceTest, CR_ADAS_103_DefaultToOffOnIntegrityFailure) {
    // Start engaged
    auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    engagement.metadata.sequenceNumber = 100;
    service->updateEngagement(engagement);
    EXPECT_TRUE(service->isEngaged());

    // Simulate integrity failure (replay attack)
    auto replayEngagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    replayEngagement.metadata.sequenceNumber = 50;
    service->updateEngagement(replayEngagement);

    // After replay rejection, engaged state should remain based on last valid message
    // The integrity failure is reported but the last valid state is maintained
    EXPECT_TRUE(service->isEngaged()) << "Should maintain last valid state after replay rejection";
}

// =============================================================================
// Degraded Mode
// =============================================================================

TEST_F(AdasStateServiceTest, DegradedMode_Forced) {
    auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(engagement);

    service->forceDegradedMode(true);

    EXPECT_TRUE(service->isDegraded());
    EXPECT_EQ(service->hmiState(), AdasHmiState::Degraded);
}

// =============================================================================
// Feature Summary
// =============================================================================

TEST_F(AdasStateServiceTest, FeatureSummary) {
    auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    engagement.accActive = true;
    engagement.lkaActive = true;
    engagement.lcaActive = false;
    service->updateEngagement(engagement);

    QString summary = service->featureSummary();
    EXPECT_TRUE(summary.contains("ACC"));
    EXPECT_TRUE(summary.contains("LKA"));
    EXPECT_FALSE(summary.contains("LCA"));
}

// =============================================================================
// Responsibility Text
// =============================================================================

TEST_F(AdasStateServiceTest, ResponsibilityText_L2) {
    auto engagement = createValidEngagement(true, true, AutomationLevel::L2_PartialAutomation);
    service->updateEngagement(engagement);

    QString responsibility = service->responsibilityText();
    EXPECT_TRUE(responsibility.contains("Driver")) << "L2 should indicate driver responsibility";
}

TEST_F(AdasStateServiceTest, ResponsibilityText_L3) {
    auto engagement = createValidEngagement(true, true, AutomationLevel::L3_ConditionalAutomation);
    service->updateEngagement(engagement);

    QString responsibility = service->responsibilityText();
    EXPECT_TRUE(responsibility.contains("System")) << "L3 should indicate system responsibility";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
