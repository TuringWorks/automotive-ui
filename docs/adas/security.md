# ADAS UI Security Architecture

## Overview

This document describes the cybersecurity architecture for the ADAS UI system, addressing threats, mitigations, and compliance with ISO/SAE 21434 and UNECE WP.29 R155.

## Threat Model

### Asset Inventory

| Asset | Criticality | Description |
|-------|-------------|-------------|
| ADAS State | High | Current automation state displayed to driver |
| Engagement Status | High | Whether system shows ENGAGED to driver |
| Takeover Requests | Critical | Driver handoff notifications |
| Perception Data | Medium | Lane, object, and environment data |
| Event Log | Medium | Historical record of system behavior |
| User Settings | Low | Driver preferences for ADAS features |

### Threat Actors

| Actor | Capability | Motivation |
|-------|------------|------------|
| Remote Attacker | Network access | Data theft, disruption |
| Local Attacker | Physical access | Vehicle theft, vandalism |
| Malicious App | App sandbox | Data exfiltration |
| Supply Chain | Build system | Backdoor insertion |

### Attack Vectors

```
┌─────────────────────────────────────────────────────────────────────┐
│                         Attack Surface                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │  IPC Channel │    │ Settings API │    │   Event Log  │          │
│  │              │    │              │    │              │          │
│  │ • Replay     │    │ • Tampering  │    │ • Deletion   │          │
│  │ • Spoofing   │    │ • Injection  │    │ • Tampering  │          │
│  │ • DoS        │    │              │    │              │          │
│  └──────────────┘    └──────────────┘    └──────────────┘          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## Security Controls

### CR-ADAS-101: Signed/Encrypted IPC Messages

**Implementation:**

All ADAS messages from ECUs include:
- HMAC-SHA256 signature
- AES-128-GCM encryption (where applicable)
- Freshness value (timestamp + counter)

```cpp
struct SecureMessage {
    uint8_t payload[MAX_PAYLOAD];
    uint32_t payloadLength;
    uint8_t hmac[32];        // HMAC-SHA256
    uint64_t timestamp;
    uint32_t sequenceNumber;
    uint32_t counter;        // Freshness counter
};

bool validateMessage(const SecureMessage& msg) {
    // Verify HMAC
    if (!verifyHmac(msg.payload, msg.payloadLength, msg.hmac)) {
        return false;
    }

    // Verify freshness
    if (msg.timestamp < lastValidTimestamp - FRESHNESS_WINDOW_MS) {
        return false;
    }

    // Verify sequence number
    if (msg.sequenceNumber <= lastValidSequence) {
        return false;
    }

    return true;
}
```

### CR-ADAS-102: Anti-Replay Protection

**Implementation:**

Sequence number validation in AdasStateService:

```cpp
bool AdasStateService::updateEngagement(const AdasEngagement& engagement) {
    // Validate sequence number
    if (engagement.metadata.sequenceNumber <= m_lastSequence) {
        emit integrityFailure("Replay attack detected",
                              engagement.metadata.sequenceNumber,
                              m_lastSequence);
        logSecurityEvent("REPLAY_REJECTED", engagement.metadata);
        return false;  // Reject replayed message
    }

    m_lastSequence = engagement.metadata.sequenceNumber;
    // Process valid message...
    return true;
}
```

**Sequence Number Rules:**
- Strictly monotonic increasing
- 32-bit counter (wraps after ~4 billion messages)
- Per-session counter reset on key exchange
- Gap detection for missing messages

### CR-ADAS-103: Fail-Safe Default

**Implementation:**

On integrity failure, maintain last valid state:

```cpp
void AdasStateService::handleIntegrityFailure() {
    // Log the failure
    logSecurityEvent("INTEGRITY_FAILURE", lastFailedMessage);

    // Emit signal for monitoring
    emit integrityFailure("Message validation failed");

    // Do NOT change state - maintain last valid state
    // The invalid message is simply discarded

    // Increment failure counter for alerting
    m_integrityFailureCount++;

    if (m_integrityFailureCount >= FAILURE_THRESHOLD) {
        // After repeated failures, transition to safe state
        forceSafeState();
    }
}
```

### CR-ADAS-104: Secure Clock Synchronization

**Implementation:**

- Hardware-backed monotonic clock
- NTP with authenticated time sources
- Fallback to local monotonic counter
- Time jump detection and handling

```cpp
uint64_t SecureClock::getCurrentTime() {
    uint64_t hwTime = getHardwareMonotonicTime();
    uint64_t ntpTime = getNtpTime();

    // Detect time jump
    if (std::abs(static_cast<int64_t>(ntpTime - hwTime)) > MAX_CLOCK_DRIFT) {
        logSecurityEvent("TIME_JUMP_DETECTED");
        return hwTime;  // Trust hardware clock
    }

    return ntpTime;
}
```

### CR-INF-ADAS-010: Tamper-Evident Logging

**Implementation:**

HmiEventLog with chained checksums:

```cpp
class HmiEventLog {
public:
    void logEvent(const HmiEvent& event) {
        // Compute chained checksum
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << event.timestamp
               << static_cast<int>(event.category)
               << static_cast<int>(event.severity)
               << event.message
               << event.data;

        QByteArray combined = m_lastChecksum + data;
        event.checksum = QCryptographicHash::hash(
            combined, QCryptographicHash::Sha256);

        m_events.append(event);
        m_lastChecksum = event.checksum;
    }

    bool verifyIntegrity() const {
        QByteArray lastChecksum;
        for (const auto& event : m_events) {
            QByteArray computed = computeChecksum(event, lastChecksum);
            if (computed != event.checksum) {
                return false;  // Tampering detected
            }
            lastChecksum = event.checksum;
        }
        return true;
    }

private:
    QList<HmiEvent> m_events;
    QByteArray m_lastChecksum;
};
```

### CR-INF-ADAS-120: Motion-Locked Engineering Mode

**Implementation:**

```qml
Item {
    id: perceptionExplorer

    property bool motionLocked: vehicleSpeed >= 5.0

    Rectangle {
        id: motionLockOverlay
        anchors.fill: parent
        visible: motionLocked
        color: "#CC000000"
        z: 100

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                text: "⚠ Vehicle in Motion"
                color: "#FFC107"
                font.pixelSize: 24
            }

            Text {
                text: "Engineering mode requires vehicle to be stopped\n(speed < 5 mph)"
                color: "white"
                font.pixelSize: 16
            }
        }
    }

    // Engineering controls disabled when motion-locked
    ColumnLayout {
        enabled: !motionLocked
        opacity: motionLocked ? 0.3 : 1.0
        // ... controls ...
    }
}
```

## Data Flow Security

### Engagement Data Path

```
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ ADAS ECU     │───▶│ IPC Gateway  │───▶│ UI Process   │
│              │    │              │    │              │
│ • Signs msg  │    │ • Verifies   │    │ • Validates  │
│ • Seq num    │    │   signature  │    │   seq num    │
│ • Timestamp  │    │ • Decrypts   │    │ • Checks     │
│              │    │              │    │   freshness  │
└──────────────┘    └──────────────┘    └──────────────┘
```

### Settings Data Path

```
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ UI Process   │───▶│ Secure Store │───▶│ Settings ECU │
│              │    │              │    │              │
│ • User input │    │ • Encrypts   │    │ • Validates  │
│ • Sanitizes  │    │ • Signs      │    │ • Applies    │
│              │    │ • Persists   │    │              │
└──────────────┘    └──────────────┘    └──────────────┘
```

## Input Validation

### ADAS Message Validation

```cpp
struct ValidationResult {
    bool valid;
    QString errorMessage;
};

ValidationResult validateAdasMessage(const AdasMessage& msg) {
    // Check metadata validity
    if (!msg.metadata.valid) {
        return {false, "Invalid metadata"};
    }

    // Check timestamp freshness
    uint64_t now = QDateTime::currentMSecsSinceEpoch();
    if (msg.metadata.timestampMs > now + CLOCK_SKEW_TOLERANCE) {
        return {false, "Timestamp in future"};
    }
    if (now - msg.metadata.timestampMs > FRESHNESS_WINDOW_MS) {
        return {false, "Message too old"};
    }

    // Check sequence number
    if (msg.metadata.sequenceNumber <= lastSequence) {
        return {false, "Invalid sequence number"};
    }

    // Check automation level range
    if (msg.level < AutomationLevel::L0_NoAutomation ||
        msg.level > AutomationLevel::L5_FullAutomation) {
        return {false, "Invalid automation level"};
    }

    // Check speed bounds
    if (msg.setSpeedMps < 0 || msg.setSpeedMps > MAX_SPEED_MPS) {
        return {false, "Invalid speed value"};
    }

    return {true, ""};
}
```

### User Input Sanitization

```cpp
QString sanitizeSettingValue(const QString& input) {
    QString sanitized = input;

    // Remove control characters
    sanitized.remove(QRegularExpression("[\\x00-\\x1F\\x7F]"));

    // Limit length
    if (sanitized.length() > MAX_SETTING_LENGTH) {
        sanitized = sanitized.left(MAX_SETTING_LENGTH);
    }

    // Encode special characters
    sanitized = sanitized.toHtmlEscaped();

    return sanitized;
}
```

## Secure State Transitions

### State Transition Validation

```cpp
bool AdasStateService::isValidTransition(AdasHmiState from, AdasHmiState to) {
    // Define allowed transitions
    static const QMap<AdasHmiState, QSet<AdasHmiState>> allowedTransitions = {
        {AdasHmiState::Off, {AdasHmiState::Available, AdasHmiState::Degraded}},
        {AdasHmiState::Available, {AdasHmiState::Off, AdasHmiState::EngagedL1,
                                   AdasHmiState::EngagedL2, AdasHmiState::Degraded}},
        {AdasHmiState::EngagedL2, {AdasHmiState::Available, AdasHmiState::EngagedL3,
                                   AdasHmiState::TakeoverRequested, AdasHmiState::Degraded}},
        // ... etc
    };

    auto it = allowedTransitions.find(from);
    if (it == allowedTransitions.end()) {
        return false;
    }

    return it.value().contains(to);
}
```

## Security Monitoring

### Event Categories

| Category | Events |
|----------|--------|
| Authentication | Key exchange, session start/end |
| Authorization | Permission checks, access denials |
| Integrity | Replay detection, checksum failures |
| Availability | DoS detection, rate limiting |
| Configuration | Setting changes, feature toggles |

### Alert Thresholds

| Event Type | Threshold | Action |
|------------|-----------|--------|
| Replay attempts | 3 in 10 sec | Log + alert |
| Checksum failures | 5 in 1 min | Log + degrade |
| Rate limit exceeded | 100 msg/sec | Rate limit |
| Time sync failure | 3 consecutive | Use local clock |

## Compliance Mapping

### ISO/SAE 21434

| Clause | Requirement | Implementation |
|--------|-------------|----------------|
| 8.3 | Threat analysis | TARA documented |
| 9.3 | Secure design | Architecture review |
| 10.4 | Secure implementation | Code review + static analysis |
| 11.3 | Verification | Security testing |

### UNECE WP.29 R155

| Annex 5 | Requirement | Implementation |
|---------|-------------|----------------|
| 7.2.2.2 | Secure communication | Signed IPC |
| 7.2.2.3 | Authenticity | HMAC verification |
| 7.2.2.4 | Integrity | Checksum validation |
| 7.2.2.5 | Availability | Rate limiting, fail-safe |

## Security Testing

### Test Categories

| Category | Description | Tools |
|----------|-------------|-------|
| Static Analysis | Code vulnerability scanning | cppcheck, clang-tidy |
| Dynamic Analysis | Runtime vulnerability detection | Valgrind, ASan |
| Fuzz Testing | Input validation testing | libFuzzer |
| Penetration Testing | Attack simulation | Custom scripts |

### Test Cases

| ID | Test | Expected Result |
|----|------|-----------------|
| SEC-001 | Replay attack | Message rejected |
| SEC-002 | Sequence rollback | Message rejected |
| SEC-003 | Invalid HMAC | Message rejected |
| SEC-004 | Future timestamp | Message rejected |
| SEC-005 | Stale timestamp | Message rejected |
| SEC-006 | Invalid state transition | Transition blocked |
| SEC-007 | Log tampering | Tampering detected |
| SEC-008 | Motion lock bypass | Access denied |

## Related Documentation

- [ADAS UI Overview](overview.md)
- [Usage Scenarios](scenarios.md)
- [Security Requirements](/docs/security/Security_Requirements.md)
- [TARA Analysis](/docs/security/TARA.md)
