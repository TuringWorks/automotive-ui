# Asset Inventory (TARA-INV-001)

Version: 1.0
Date: 2025-12-23
Status: Draft
Document ID: TARA-INV-001

---

## 1. Introduction

### 1.1 Purpose

This document catalogs all cybersecurity-relevant assets within the Driver UI and Infotainment UI components to support Threat Analysis and Risk Assessment (TARA) activities.

### 1.2 Asset Classification

Assets are classified by:
- **Confidentiality**: Sensitivity of information
- **Integrity**: Importance of data accuracy
- **Availability**: Criticality of service uptime

---

## 2. Asset Catalog

### 2.1 Data Assets

| Asset ID | Asset Name | Location | Confidentiality | Integrity | Availability | Owner |
|----------|------------|----------|-----------------|-----------|--------------|-------|
| DA-001 | User profiles | SecureSettings | Medium | High | Medium | Infotainment |
| DA-002 | Media preferences | SecureSettings | Low | Medium | Low | Infotainment |
| DA-003 | Navigation history | SecureSettings | Medium | Medium | Low | Infotainment |
| DA-004 | Bluetooth pairings | SecureSettings | High | High | Medium | Infotainment |
| DA-005 | Phone contacts | PhoneController | High | High | Medium | Infotainment |
| DA-006 | Call history | PhoneController | High | Medium | Low | Infotainment |
| DA-007 | WiFi credentials | (Future) | High | High | Medium | Infotainment |
| DA-008 | Vehicle settings | VehicleController | Medium | High | High | Infotainment |
| DA-009 | Drive mode config | VehicleController | Low | High | High | Vehicle |

### 2.2 Software Assets

| Asset ID | Asset Name | Location | Confidentiality | Integrity | Availability | Owner |
|----------|------------|----------|-----------------|-----------|--------------|-------|
| SA-001 | Infotainment binary | /app/ | Low | Critical | High | Build |
| SA-002 | Driver UI binary | /app/ | Low | Critical | Critical | Build |
| SA-003 | QML UI files | /qml/ | Low | High | High | Build |
| SA-004 | Plugin modules | (Future) | Low | Critical | Medium | Build |
| SA-005 | Configuration files | /config/ | Medium | High | High | Deploy |
| SA-006 | Log files | /var/log/ | Medium | Medium | Low | Runtime |

### 2.3 Communication Assets

| Asset ID | Asset Name | Protocol | Confidentiality | Integrity | Availability | Owner |
|----------|------------|----------|-----------------|-----------|--------------|-------|
| CA-001 | IPC Channel (Signals) | Unix socket | Low | Critical | Critical | Shared |
| CA-002 | IPC Channel (Settings) | Unix socket | Medium | High | High | Shared |
| CA-003 | IPC Channel (Alerts) | Unix socket | Low | High | Critical | Shared |
| CA-004 | Bluetooth connection | BT | Medium | Medium | Medium | Infotainment |
| CA-005 | USB connection | USB | Low | High | Low | Infotainment |

### 2.4 Service Assets

| Asset ID | Asset Name | Component | Confidentiality | Integrity | Availability | Owner |
|----------|------------|-----------|-----------------|-----------|--------------|-------|
| SV-001 | SignalHub service | Shared | Low | Critical | Critical | Platform |
| SV-002 | PermissionManager | Infotainment | Medium | High | High | Security |
| SV-003 | SecureSettings | Shared | Medium | High | High | Security |
| SV-004 | Logger service | Shared | Medium | Medium | Medium | Platform |
| SV-005 | MediaController | Infotainment | Low | Medium | Medium | Infotainment |
| SV-006 | NavigationController | Infotainment | Medium | High | Medium | Infotainment |
| SV-007 | PhoneController | Infotainment | High | High | Medium | Infotainment |
| SV-008 | VehicleController | Infotainment | Medium | Critical | High | Vehicle |

### 2.5 Cryptographic Assets

| Asset ID | Asset Name | Location | Confidentiality | Integrity | Availability | Owner |
|----------|------------|----------|-----------------|-----------|--------------|-------|
| CR-001 | Settings encryption key | Secure storage | Critical | Critical | High | Security |
| CR-002 | IPC signing key (future) | Secure storage | Critical | Critical | High | Security |
| CR-003 | Plugin signing cert (future) | Secure storage | High | Critical | High | Security |
| CR-004 | Communication tokens | Memory | High | High | Medium | Security |

---

## 3. Asset Relationships

### 3.1 Data Flow Diagram

```text
┌─────────────────────────────────────────────────────--──┐
│                        External Systems                 │
├─────────────────────────────────────────────────────────┤
│  [Bluetooth]    [USB]    [CAN Bus]    [Cloud] (future)  │
│      │           │           │              │           │
└──────┼───────────┼───────────┼──────────────┼───────────┘
       │           │           │              │
       ▼           ▼           ▼              ▼
┌─────────────────────────────────────────────────────────┐
│                     Infotainment UI                     │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐        │
│  │ Phone       │ │ Media       │ │ Navigation  │        │
│  │ Controller  │ │ Controller  │ │ Controller  │        │
│  │  (DA-005,6) │ │  (DA-002)   │ │  (DA-003)   │        │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘        │
│         │               │               │               │
│         └───────────────┼───────────────┘               │
│                         ▼                               │
│              ┌─────────────────────┐                    │
│              │ Permission Manager  │                    │
│              │    (SV-002)         │                    │
│              └──────────┬──────────┘                    │
└─────────────────────────┼───────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                      Shared Services                    │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐        │
│  │ IPC Channel │ │ Secure      │ │ Logger      │        │
│  │  (CA-001-3) │ │ Settings    │ │  (SV-004)   │        │
│  └──────┬──────┘ │  (SV-003)   │ └─────────────┘        │
└─────────┼───────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────┐
│                       Driver UI                         │
│  ┌────────────────────────────────────────────────────┐ │
│  │            Signal Consumer (read-only)             │ │
│  └────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### 3.2 Trust Boundaries

| Boundary | Components | Trust Level |
|----------|------------|-------------|
| TB-001 | External ↔ Infotainment | Untrusted |
| TB-002 | Infotainment ↔ Shared | Semi-trusted |
| TB-003 | Shared ↔ Driver UI | Trusted |
| TB-004 | Any ↔ Vehicle CAN | Trusted (assumed) |

---

## 4. Asset Criticality Analysis

### 4.1 Critical Assets (Highest Priority)

| Asset | Criticality | Justification |
|-------|-------------|---------------|
| CA-001 (Signal IPC) | Critical | Affects safety display |
| SA-002 (Driver UI binary) | Critical | Safety-relevant rendering |
| SV-001 (SignalHub) | Critical | Signal integrity |
| DA-008 (Vehicle settings) | Critical | Vehicle control |

### 4.2 High Priority Assets

| Asset | Priority | Justification |
|-------|----------|---------------|
| DA-005 (Contacts) | High | Privacy impact |
| DA-006 (Call history) | High | Privacy impact |
| CR-001 (Encryption key) | High | Protects all settings |
| SV-002 (PermissionManager) | High | Access control |

### 4.3 Medium Priority Assets

| Asset | Priority | Justification |
|-------|----------|---------------|
| DA-001 (User profiles) | Medium | User data |
| DA-003 (Nav history) | Medium | Privacy |
| SA-006 (Log files) | Medium | May contain PII |

---

## 5. Data Sensitivity

### 5.1 Personal Identifiable Information (PII)

| Asset | PII Elements | Protection Required |
|-------|--------------|---------------------|
| DA-005 (Contacts) | Names, phone numbers, addresses | Encryption at rest |
| DA-006 (Call history) | Phone numbers, timestamps | Encryption at rest |
| DA-003 (Nav history) | Location data | Encryption, time-limited retention |
| SA-006 (Log files) | May contain any PII | Redaction before logging |

### 5.2 Sensitive Non-PII

| Asset | Sensitive Elements | Protection Required |
|-------|-------------------|---------------------|
| DA-004 (BT pairings) | Device identifiers | Access control |
| DA-008 (Vehicle settings) | Drive mode, HVAC | Permission gating |
| CR-001-004 (Crypto assets) | Keys, certificates | Secure storage |

---

## 6. Asset Lifecycle

### 6.1 Data Retention

| Asset | Retention Period | Disposal Method |
|-------|------------------|-----------------|
| Call history | 30 days | Secure delete |
| Navigation history | 90 days | Secure delete |
| Log files | 7 days | Overwrite |
| User profiles | Until factory reset | Secure wipe |

### 6.2 Asset Provisioning

| Asset | Provisioning | Responsible |
|-------|--------------|-------------|
| Encryption keys | Generated on first boot | Security service |
| User profiles | Created on setup | User |
| Configuration | Factory defaults + user | Deployment |

---

## 7. References

- ISO/SAE 21434 Clause 8 - Threat Analysis and Risk Assessment
- GDPR Article 4 - PII Definition
- Internal Data Classification Policy

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Security Engineer | Initial release |
