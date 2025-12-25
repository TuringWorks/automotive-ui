// MockSignalHub.h

#pragma once

#include <gmock/gmock.h>
#include <string>

class MockSignalHub {
public:
    MOCK_METHOD(void, emitSignal, (const std::string& name, double value), ());
    MOCK_METHOD(double, getSignal, (const std::string& name), (const));
};
