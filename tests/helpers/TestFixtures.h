// TestFixtures.h

#pragma once

#include <gtest/gtest.h>

class SafetyTestFixture : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class SecurityTestFixture : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};
