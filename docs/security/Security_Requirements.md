# Security Requirements (CS-REQ-001)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: CS-REQ-001

---

## 1. Introduction

### 1.1 Purpose

This document specifies the cybersecurity requirements for the Automotive UI Suite, derived from the Threat Analysis and Risk Assessment (TARA) and aligned with ISO/SAE 21434 and UNECE R155 requirements.

### 1.2 Scope

Requirements cover:
- IPC integrity (CR-INF-001)
- Permission enforcement (CR-INF-002)
- Plugin integrity (CR-INF-003)
- PII protection (CR-INF-004)

---

## 2. Security Requirements

### 2.1 CR-INF-001: IPC Message Integrity

| Attribute | Value |
|-----------|-------|
| **Requirement ID** | CR-INF-001 |
| **Title** | IPC Message Integrity and Versioning |
| **Priority** | High |
| **CSG Reference** | CSG-001 |
| **TARA Reference** | TARA-001, TARA-007 |

**Requirement Statement**:
IPC messages shall include versioning and integrity verification mechanisms. Malformed or invalid messages shall be rejected without processing, and the rejection shall be logged.

**Detailed Requirements**:

| Sub-ID | Requirement | Verification |
|--------|-------------|--------------|
| CR-INF-001.1 | All IPC messages shall include a protocol version field | Code review |
| CR-INF-001.2 | All IPC messages shall include a message type identifier | Code review |
| CR-INF-001.3 | Messages with unknown version shall be rejected | Unit test |
| CR-INF-001.4 | Messages with invalid structure shall be rejected | Fuzz test |
| CR-INF-001.5 | Message rejection shall generate audit log entry | Unit test |
| CR-INF-001.6 | (Future) Messages shall include cryptographic integrity check | Design review |

**Implementation**:
```cpp
// IpcMessage.h
struct IpcMessage {
    uint32_t version;        // CR-INF-001.1
    MessageType type;        // CR-INF-001.2
    uint32_t payloadSize;
    std::vector<uint8_t> payload;
    // Future: uint32_t checksum;
};

// Validation
bool IpcChannel::validateMessage(const IpcMessage& msg) {
    if (msg.version < MIN_VERSION || msg.version > MAX_VERSION) {
        Logger::security("IPC", "Rejected: invalid version");
        return false;  // CR-INF-001.3
    }
    if (!isValidType(msg.type)) {
        Logger::security("IPC", "Rejected: invalid type");
        return false;  // CR-INF-001.4
    }
    return true;
}
```

---

### 2.2 CR-INF-002: Permission Enforcement

| Attribute | Value |
|-----------|-------|
| **Requirement ID** | CR-INF-002 |
| **Title** | Privileged Action Permission Enforcement |
| **Priority** | High |
| **CSG Reference** | CSG-002 |
| **TARA Reference** | TARA-002, TARA-010 |

**Requirement Statement**:
Privileged actions shall require explicit permission checks. All permission decisions (grant and deny) shall generate audit events.

**Detailed Requirements**:

| Sub-ID | Requirement | Verification |
|--------|-------------|--------------|
| CR-INF-002.1 | Each privileged operation shall have an associated permission | Design review |
| CR-INF-002.2 | Permission check shall occur before privileged operation | Code review |
| CR-INF-002.3 | Denied permission shall prevent operation execution | Unit test |
| CR-INF-002.4 | Permission grant shall be logged | Unit test |
| CR-INF-002.5 | Permission denial shall be logged | Unit test |
| CR-INF-002.6 | Permission state shall not be bypassable | Penetration test |

**Privileged Operations**:

| Operation | Required Permission | Category |
|-----------|---------------------|----------|
| setDriveMode() | VehicleDriveModes | Vehicle Control |
| setVehicleSetting() | VehicleSettingsWrite | Vehicle Control |
| accessContacts() | PhoneContacts | Privacy |
| accessCallHistory() | PhoneHistory | Privacy |
| modifySystemSettings() | SystemSettings | System |
| enableDebugMode() | DebugAccess | Development |

**Implementation**:
```cpp
// PermissionManager.cpp
bool PermissionManager::checkPermission(Permission perm, const QString& action) {
    bool granted = hasPermission(perm);

    // CR-INF-002.4, CR-INF-002.5: Audit logging
    if (granted) {
        emit permissionGranted(action);
        Logger::security("Permission", QString("GRANTED: %1").arg(action));
    } else {
        emit permissionDenied(action);
        Logger::security("Permission", QString("DENIED: %1").arg(action));
    }

    return granted;
}

// Usage in VehicleSettingsController
bool VehicleSettingsController::setDriveMode(const QString& mode) {
    // CR-INF-002.2: Check before operation
    if (!m_permissionManager->checkPermission(
            Permission::VehicleDriveModes, "setDriveMode")) {
        return false;  // CR-INF-002.3: Prevent execution
    }

    // Proceed with operation
    m_driveMode = mode;
    emit driveModeChanged();
    return true;
}
```

---

### 2.3 CR-INF-003: Plugin Integrity

| Attribute | Value |
|-----------|-------|
| **Requirement ID** | CR-INF-003 |
| **Title** | Plugin Signature Verification |
| **Priority** | High |
| **CSG Reference** | CSG-003 |
| **TARA Reference** | TARA-003 |

**Requirement Statement**:
Plugins shall only be loaded from signed manifests. Unsigned or tampered plugins shall not be loaded.

**Note**: This requirement is scaffolded in v1 and will be fully implemented in v2.

**Detailed Requirements**:

| Sub-ID | Requirement | Verification | Status |
|--------|-------------|--------------|--------|
| CR-INF-003.1 | Plugin manifests shall support digital signatures | Design review | v2 |
| CR-INF-003.2 | Plugin loader shall verify signature before loading | Unit test | v2 |
| CR-INF-003.3 | Invalid signature shall prevent plugin loading | Unit test | v2 |
| CR-INF-003.4 | Plugin loading attempts shall be logged | Unit test | v1 |
| CR-INF-003.5 | Plugin loading shall be configurable (enabled/disabled) | Unit test | v1 |

**v1 Implementation** (Scaffold):
```cpp
// PluginLoader.h (v1 scaffold)
class PluginLoader {
public:
    bool loadPlugin(const QString& path) {
        // CR-INF-003.4: Log attempt
        Logger::info("Plugin", QString("Loading attempt: %1").arg(path));

        // CR-INF-003.5: Check if enabled
        if (!m_pluginsEnabled) {
            Logger::warning("Plugin", "Plugin loading disabled");
            return false;
        }

        // v1: Log warning about missing signature verification
        Logger::warning("Plugin",
            "SECURITY: Plugin signature verification not implemented");

        // TODO v2: Implement signature verification (CR-INF-003.2)
        return false;  // v1: Plugins disabled by default
    }
};
```

---

### 2.4 CR-INF-004: PII Redaction

| Attribute | Value |
|-----------|-------|
| **Requirement ID** | CR-INF-004 |
| **Title** | PII Redaction in Logs |
| **Priority** | High |
| **CSG Reference** | CSG-004 |
| **TARA Reference** | TARA-004 |

**Requirement Statement**:
Logs shall redact PII by default. Debug logging with PII shall be gated by a compile-time flag.

**Detailed Requirements**:

| Sub-ID | Requirement | Verification |
|--------|-------------|--------------|
| CR-INF-004.1 | PII patterns shall be identified and cataloged | Design review |
| CR-INF-004.2 | Logger shall apply PII redaction before output | Unit test |
| CR-INF-004.3 | Redaction shall replace PII with placeholder | Unit test |
| CR-INF-004.4 | Debug logging with PII shall require DEBUG_PII flag | Build review |
| CR-INF-004.5 | Redaction bypass shall not be possible at runtime | Code review |

**PII Patterns**:

| Pattern Type | Regex | Redaction |
|--------------|-------|-----------|
| Email | `\b[\w.-]+@[\w.-]+\.\w+\b` | `[EMAIL]` |
| Phone | `\b\d{3}[-.]?\d{3}[-.]?\d{4}\b` | `[PHONE]` |
| Credit Card | `\b\d{4}[- ]?\d{4}[- ]?\d{4}[- ]?\d{4}\b` | `[CARD]` |
| SSN | `\b\d{3}-\d{2}-\d{4}\b` | `[SSN]` |
| IP Address | `\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b` | `[IP]` |
| VIN | `\b[A-HJ-NPR-Z0-9]{17}\b` | `[VIN]` |

**Implementation**:
```cpp
// PiiRedactor.cpp
QString PiiRedactor::redact(const QString& input) {
    QString result = input;

    // CR-INF-004.4: Check debug flag
    #ifdef DEBUG_PII_LOGGING
        return result;  // No redaction in debug builds
    #endif

    // CR-INF-004.2, CR-INF-004.3: Apply redaction
    for (const auto& [pattern, replacement] : m_patterns) {
        result.replace(pattern, replacement);
    }

    return result;
}

// Logger integration
void Logger::info(const QString& category, const QString& message) {
    QString redacted = PiiRedactor::instance().redact(message);
    // Output redacted message
}
```

---

## 3. Additional Security Requirements

### 3.1 CR-SEC-001: Secure Defaults

| Sub-ID | Requirement |
|--------|-------------|
| CR-SEC-001.1 | All security features shall be enabled by default |
| CR-SEC-001.2 | Debug features shall be disabled by default |
| CR-SEC-001.3 | Permissions shall deny by default |

### 3.2 CR-SEC-002: Error Handling

| Sub-ID | Requirement |
|--------|-------------|
| CR-SEC-002.1 | Security errors shall not expose sensitive details |
| CR-SEC-002.2 | Failed operations shall fail safely |
| CR-SEC-002.3 | Error messages shall be logged with context |

### 3.3 CR-SEC-003: Input Validation

| Sub-ID | Requirement |
|--------|-------------|
| CR-SEC-003.1 | All external input shall be validated |
| CR-SEC-003.2 | Input validation shall occur at trust boundaries |
| CR-SEC-003.3 | Invalid input shall be rejected, not sanitized |

---

## 4. Verification Matrix

| Requirement | Review | Analysis | Test | Penetration |
|-------------|--------|----------|------|-------------|
| CR-INF-001.1 | X | | | |
| CR-INF-001.2 | X | | | |
| CR-INF-001.3 | | | X | |
| CR-INF-001.4 | | | X (fuzz) | |
| CR-INF-001.5 | | | X | |
| CR-INF-002.1 | X | | | |
| CR-INF-002.2 | X | | | |
| CR-INF-002.3 | | | X | |
| CR-INF-002.4 | | | X | |
| CR-INF-002.5 | | | X | |
| CR-INF-002.6 | | | | X |
| CR-INF-003.4 | | | X | |
| CR-INF-003.5 | | | X | |
| CR-INF-004.1 | X | | | |
| CR-INF-004.2 | | | X | |
| CR-INF-004.3 | | | X | |
| CR-INF-004.4 | X | | | |

---

## 5. Traceability

### 5.1 CSG to CR Mapping

| CSG | CR Requirements |
|-----|-----------------|
| CSG-001 | CR-INF-001 |
| CSG-002 | CR-INF-002 |
| CSG-003 | CR-INF-003 |
| CSG-004 | CR-INF-004 |

### 5.2 CR to Implementation Mapping

| CR | Source Files |
|----|--------------|
| CR-INF-001 | IpcMessage.h/.cpp, IpcChannel.h/.cpp |
| CR-INF-002 | PermissionManager.h/.cpp, *Controller.cpp |
| CR-INF-003 | PluginLoader.h/.cpp (scaffold) |
| CR-INF-004 | PiiRedactor.h/.cpp, Logger.h/.cpp |

---

## 6. References

- CS-TARA-001: Threat Analysis and Risk Assessment
- CS-TEST-001: Security Test Plan
- ISO/SAE 21434 Clause 9: Product Development

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Security Engineer | Initial release |
