# Technical Safety Concept (TSC)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: SAFE-TSC-001

---

## 1. Introduction

### 1.1 Purpose

This document defines the Technical Safety Concept (TSC) for the Driver UI component, specifying technical safety requirements and their implementation in the software architecture.

### 1.2 Scope

Covers technical implementation of:
- Signal validation algorithms
- Safety monitoring mechanisms
- Degraded mode implementation
- Memory and timing constraints

---

## 2. Technical Safety Requirements

### 2.1 Signal Validation (from FSR-001)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-001 | Speed range check: 0 ≤ speed ≤ 300 km/h | ASIL C | SignalValidator::validateRange() |
| TSR-002 | Speed rate-of-change check: Δv/Δt ≤ 15 m/s² | ASIL C | SignalValidator::validateRateOfChange() |
| TSR-003 | Speed freshness check: age ≤ 300ms | ASIL C | SignalHub::checkFreshness() |

**Implementation Details**:

```cpp
// TSR-001: Range validation
bool SignalValidator::validateRange(const SignalValue& signal) {
    const auto& def = m_definitions[signal.id];
    if (signal.value < def.minValue || signal.value > def.maxValue) {
        return false;  // Out of range
    }
    return true;
}

// TSR-002: Rate of change validation
bool SignalValidator::validateRateOfChange(const SignalValue& signal) {
    const auto& def = m_definitions[signal.id];
    if (def.maxRateOfChange <= 0) return true;  // Not configured

    double dt = (signal.timestamp - m_lastTimestamp[signal.id]) / 1000.0;
    if (dt <= 0) return true;  // First sample or time issue

    double rate = std::abs(signal.value - m_lastValue[signal.id]) / dt;
    return rate <= def.maxRateOfChange;
}

// TSR-003: Freshness validation
SignalValidity SignalHub::checkFreshness(SignalId id, int64_t currentTime) {
    const auto& signal = m_signals[id];
    int64_t age = currentTime - signal.timestamp;

    if (age > m_definitions[id].stalenessThresholdMs) {
        return SignalValidity::Stale;
    }
    return signal.validity;
}
```

### 2.2 Validity Indication (from FSR-002)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-004 | Invalid speed shall display "---" within 100ms | ASIL C | ClusterStateModel, ClusterView.qml |
| TSR-005 | Stale speed shall show staleness indicator | ASIL C | ClusterStateModel::speedStale property |

**QML Binding (Display Only)**:

```qml
// Display reacts to C++ validated state - no logic in QML
Text {
    text: clusterViewModel.speedValid ?
          clusterViewModel.speed.toFixed(0) : "---"
    opacity: clusterViewModel.speedStale ? 0.5 : 1.0
}

// Stale indicator
Rectangle {
    visible: clusterViewModel.speedStale
    // Visual stale indicator
}
```

### 2.3 Telltale Management (from FSR-003)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-006 | Critical telltales rendered in z-order priority | ASIL B | TelltaleBar with fixed z-order |
| TSR-007 | Alert overlay shall not obscure telltale bar | ASIL B | AlertOverlay layout constraints |

**Architecture**:

```
┌───────────────────────────────────────────────────────────┐
│                     Screen Layout                         │
├───────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────┐  │
│  │     Telltale Bar (z=100, NEVER obscured)            │  │
│  └─────────────────────────────────────────────────────┘  │
│  ┌─────────────────────────────────────────────────────┐  │
│  │     Alert Area (z=50, below telltales)              │  │
│  └─────────────────────────────────────────────────────┘  │
│  ┌─────────────────────────────────────────────────────┐  │
│  │     Main Content (z=0)                              │  │
│  └─────────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────────┘
```

### 2.4 ADAS Status (from FSR-004)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-008 | ADAS indicator default to inactive; active only on valid signal | ASIL B | ClusterStateModel ADAS properties |

**State Machine**:

```
                    ┌──────────┐
        ──────────► │ Inactive │ ◄─────── Default
                    └────┬─────┘
                         │ Valid signal + active status
                         ▼
                    ┌──────────┐
                    │  Active  │
                    └────┬─────┘
                         │ Invalid/stale signal OR inactive status
                         ▼
                    ┌──────────┐
                    │ Inactive │
                    └──────────┘
```

### 2.5 Display Health (from FSR-005)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-009 | Frame heartbeat monitored at 60Hz | ASIL A | SafetyMonitor::onFrameRendered() |
| TSR-010 | >10 consecutive missed frames triggers degraded mode | ASIL A | SafetyMonitor, DegradedModeController |

**Implementation**:

```cpp
void SafetyMonitor::onFrameRendered() {
    m_lastFrameTime = m_timeSource->currentTimeMs();
    m_consecutiveMissedFrames = 0;
}

void SafetyMonitor::checkFrameHealth() {
    int64_t now = m_timeSource->currentTimeMs();
    int64_t elapsed = now - m_lastFrameTime;

    if (elapsed > FRAME_INTERVAL_MS * 2) {  // Missed frame
        m_consecutiveMissedFrames++;

        if (m_consecutiveMissedFrames > MISSED_FRAME_THRESHOLD) {
            m_degradedController->requestDegradation(
                DegradedLevel::Degraded,
                "Frame render failure"
            );
        }
    }
}
```

### 2.6 Gear Indication (from FSR-006)

| TSR ID | Requirement | ASIL | Implementation |
|--------|-------------|------|----------------|
| TSR-011 | Invalid gear signal displays "?" indicator | ASIL B | ClusterStateModel::gearDisplay |

---

## 3. Safety Architecture Implementation

### 3.1 Memory Management

**Requirement**: No dynamic memory allocation in safety-critical paths after initialization.

| Component | Allocation Strategy | Max Size |
|-----------|---------------------|----------|
| Signal buffers | Pre-allocated array | 64 signals |
| Alert queue | Fixed ring buffer | 16 alerts |
| Telltale state | Static array | 32 telltales |
| State model | Stack/static members | ~1KB |

**Implementation Pattern**:

```cpp
// Pre-allocated signal storage
class SignalHub {
private:
    // Fixed-size arrays, no heap allocation at runtime
    std::array<SignalValue, MAX_SIGNALS> m_signals;
    std::array<SignalDefinition, MAX_SIGNALS> m_definitions;
};

// Fixed-size alert queue
class AlertManager {
private:
    static constexpr size_t MAX_ALERTS = 16;
    std::array<Alert, MAX_ALERTS> m_alertPool;
    // Ring buffer indices
    size_t m_head = 0;
    size_t m_tail = 0;
};
```

### 3.2 Timing Architecture

**Requirement**: Deterministic update cycle with bounded execution time.

| Phase | Target | Max Duration | WCET Analysis |
|-------|--------|--------------|---------------|
| Signal receive | 20 Hz | 5 ms | Static analysis |
| Validation | Per signal | 100 μs | Measured |
| State update | 20 Hz | 2 ms | Static analysis |
| QML render | 60 Hz | 16.7 ms | Qt framework |

**Scheduler Integration**:

```cpp
// Deterministic scheduling
void ClusterApplication::initScheduler() {
    // Signal processing at 20Hz (50ms period)
    m_scheduler->scheduleTask(
        "signal_processing",
        std::bind(&ClusterApplication::processSignals, this),
        50  // ms period
    );

    // Safety monitoring at 10Hz
    m_scheduler->scheduleTask(
        "safety_monitor",
        std::bind(&SafetyMonitor::runCycle, m_safetyMonitor.get()),
        100  // ms period
    );
}
```

### 3.3 Watchdog Implementation

**Requirement**: Software watchdog for render loop health.

```cpp
class SafetyMonitor {
public:
    void startWatchdog() {
        m_watchdogTimer = std::make_unique<QTimer>();
        m_watchdogTimer->setInterval(WATCHDOG_PERIOD_MS);  // 100ms
        connect(m_watchdogTimer.get(), &QTimer::timeout,
                this, &SafetyMonitor::onWatchdogTimeout);
        m_watchdogTimer->start();
    }

    void feedWatchdog() {
        m_lastFeedTime = m_timeSource->currentTimeMs();
    }

private:
    void onWatchdogTimeout() {
        int64_t elapsed = m_timeSource->currentTimeMs() - m_lastFeedTime;
        if (elapsed > WATCHDOG_PERIOD_MS * 2) {
            // Watchdog expired - take action
            m_degradedController->requestDegradation(
                DegradedLevel::Minimal,
                "Watchdog timeout"
            );
        }
    }
};
```

### 3.4 Plausibility Checks

**Requirement**: Cross-signal consistency validation.

| Check | Signals | Rule | Action |
|-------|---------|------|--------|
| Speed vs RPM | Speed, RPM, Gear | Speed should correlate with RPM*GearRatio | Log warning if deviation >20% |
| Fuel consistency | Fuel level, Distance | Fuel should not increase (unless refuel) | Mark suspect if increases without stop |

```cpp
bool SignalValidator::validatePlausibility(const SignalValue& speed,
                                           const SignalValue& rpm,
                                           const SignalValue& gear) {
    if (!isValid(speed) || !isValid(rpm) || !isValid(gear)) {
        return true;  // Cannot validate, pass through
    }

    double expectedSpeed = calculateExpectedSpeed(rpm.value, gear.value);
    double deviation = std::abs(speed.value - expectedSpeed) / expectedSpeed;

    if (deviation > PLAUSIBILITY_THRESHOLD) {
        emit plausibilityWarning("Speed/RPM mismatch", deviation);
        return false;
    }
    return true;
}
```

---

## 4. Degraded Mode Implementation

### 4.1 State Machine

```cpp
enum class DegradedLevel : uint8_t {
    Normal = 0,   // All signals valid
    Partial = 1,  // 1-2 signals degraded
    Degraded = 2, // 3+ signals degraded
    Minimal = 3   // Complete signal loss or render failure
};
```

### 4.2 Display Content by Level

| Level | Speed | RPM | Telltales | Alerts | Additional |
|-------|-------|-----|-----------|--------|------------|
| Normal | Full gauge | Full gauge | All | All | Full content |
| Partial | With indicator | With indicator | Critical | Critical | Reduced |
| Degraded | Last valid + warning | Hidden | Critical only | Critical only | Warning banner |
| Minimal | Hidden | Hidden | None | Fault message | Full fault screen |

### 4.3 Transition Logic

```cpp
void DegradedModeController::evaluateState() {
    int staleCount = 0;
    int invalidCount = 0;

    for (const auto& signal : m_criticalSignals) {
        auto validity = m_signalHub->getValidity(signal);
        if (validity == SignalValidity::Stale) staleCount++;
        if (validity == SignalValidity::Invalid) invalidCount++;
    }

    DegradedLevel targetLevel;
    if (invalidCount > 0 || staleCount >= 3) {
        targetLevel = DegradedLevel::Degraded;
    } else if (staleCount > 0) {
        targetLevel = DegradedLevel::Partial;
    } else {
        targetLevel = DegradedLevel::Normal;
    }

    transitionTo(targetLevel);
}
```

---

## 5. Fault Injection Support

### 5.1 Test Scenarios

| Scenario ID | Description | Injected Fault | Expected Result |
|-------------|-------------|----------------|-----------------|
| FI-001 | Stale speed | No speed update for 500ms | Speed shows stale indicator |
| FI-002 | Out of range speed | Speed = 500 km/h | Speed clamped, invalid shown |
| FI-003 | Multiple stale | 3+ signals stale | Degraded mode activated |
| FI-004 | Complete loss | All signals timeout | Minimal mode activated |
| FI-005 | Rapid fluctuation | Speed oscillates wildly | Rate-of-change rejection |

### 5.2 Fault Injector API

```cpp
class FaultInjector {
public:
    void injectFault(FaultScenario scenario);
    void clearFault(FaultScenario scenario);
    void clearAllFaults();

    bool isFaultActive(FaultScenario scenario) const;
    QStringList activeFaults() const;

signals:
    void faultInjected(FaultScenario scenario);
    void faultCleared(FaultScenario scenario);
};
```

---

## 6. Hardware/OS Assumptions

### 6.1 Platform Requirements

| Requirement | Specification | Verification |
|-------------|---------------|--------------|
| Display refresh | ≥60 Hz | Hardware spec review |
| GPU driver | OpenGL ES 3.0+ stable | Driver qualification |
| OS scheduler | SCHED_FIFO available | Platform test |
| Timer resolution | ≤1 ms | Platform measurement |
| Memory | ≥256 MB available | Resource monitoring |

### 6.2 External Dependencies

| Dependency | Version | Safety Relevance |
|------------|---------|------------------|
| Qt Framework | 6.5+ | UI rendering |
| C++ Standard Library | C++17 | Core utilities |
| Operating System | Linux/Windows/macOS | Scheduling, timing |
| GPU Driver | Platform-specific | Render stability |

---

## 7. Traceability

### 7.1 FSR to TSR Mapping

| FSR | TSR(s) | Component(s) |
|-----|--------|--------------|
| FSR-001 | TSR-001, TSR-002, TSR-003 | SignalValidator, SignalHub |
| FSR-002 | TSR-004, TSR-005 | ClusterStateModel, QML |
| FSR-003 | TSR-006, TSR-007 | TelltaleBar, AlertOverlay |
| FSR-004 | TSR-008 | ClusterStateModel |
| FSR-005 | TSR-009, TSR-010 | SafetyMonitor, DegradedModeController |
| FSR-006 | TSR-011 | ClusterStateModel |

### 7.2 TSR to Code Mapping

| TSR | Source File | Function/Class |
|-----|-------------|----------------|
| TSR-001 | SignalValidator.cpp | validateRange() |
| TSR-002 | SignalValidator.cpp | validateRateOfChange() |
| TSR-003 | SignalHub.cpp | checkFreshness() |
| TSR-004 | ClusterStateModel.cpp | updateSpeed() |
| TSR-005 | ClusterStateModel.cpp | speedStale property |
| TSR-006 | TelltaleBar.qml | z-order management |
| TSR-007 | AlertOverlay.qml | layout constraints |
| TSR-008 | ClusterStateModel.cpp | ADAS properties |
| TSR-009 | SafetyMonitor.cpp | onFrameRendered() |
| TSR-010 | SafetyMonitor.cpp | checkFrameHealth() |
| TSR-011 | ClusterStateModel.cpp | gearDisplay property |

---

## 8. References

- SAFE-FSC-001: Functional Safety Concept
- SAFE-HARA-001: Hazard Analysis and Risk Assessment
- SAFE-VER-001: Verification Plan

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Safety Engineer | Initial release |
