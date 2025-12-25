// MockTimeSource.h

#pragma once

#include <gmock/gmock.h>
#include <cstdint>

class MockTimeSource {
public:
    MOCK_METHOD(int64_t, currentTimeMs, (), (const));
    MOCK_METHOD(void, advanceTime, (int64_t ms), ());
};
