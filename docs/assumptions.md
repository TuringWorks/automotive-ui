# Project Assumptions

Version: 1.0
Date: 2025-12-23

This document captures the key assumptions made during the design and implementation of the Automotive UI Suite.

---

## 1. Safety Assumptions

### 1.1 Signal Source Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-S-001 | Vehicle signals originate from certified ECUs | Upstream responsibility | System integration test |
| ASM-S-002 | Signal truth is established before UI receives data | UI is display-only | Architecture review |
| ASM-S-003 | CAN bus timing is deterministic within spec | Standard automotive assumption | Platform qualification |
| ASM-S-004 | Signal validation occurs in trusted domain | Safety partitioning | Design review |

### 1.2 Hardware Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-H-001 | Display refresh rate ≥60 Hz | Smooth UI requirement | Hardware specification |
| ASM-H-002 | GPU driver is stable and qualified | Platform dependency | Supplier qualification |
| ASM-H-003 | Memory ECC/parity if required by ASIL | Hardware safety | Hardware design review |
| ASM-H-004 | Sufficient CPU headroom for 20Hz signal processing | Performance requirement | Load testing |

### 1.3 Software Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-SW-001 | Qt 6.5+ is used and qualified | Framework dependency | Tool qualification |
| ASM-SW-002 | C++17 compiler with no undefined behavior | Language standard | Compiler qualification |
| ASM-SW-003 | OS provides deterministic scheduling | Real-time requirement | Platform test |
| ASM-SW-004 | No dynamic memory allocation in safety path after init | Determinism | Code review |

---

## 2. Security Assumptions

### 2.1 Threat Model Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-C-001 | Desktop build is development-only | Production has HW security | Deployment control |
| ASM-C-002 | IPC messages are untrusted | Defense in depth | Design review |
| ASM-C-003 | Vehicle CAN bus is trusted | Out of scope | Scope definition |
| ASM-C-004 | Attacker may have local code execution | Conservative assumption | Threat model |

### 2.2 External Dependency Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-E-001 | Qt libraries have no critical vulnerabilities | Dependency risk | CVE monitoring |
| ASM-E-002 | Compiler generates expected output | Toolchain trust | Qualification |
| ASM-E-003 | OS provides process isolation | Security boundary | Platform test |

---

## 3. Operational Assumptions

### 3.1 Usage Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-O-001 | Driver glances at display, not continuous viewing | Human factors | User studies |
| ASM-O-002 | Vehicle is used in standard automotive conditions | Operating envelope | Requirements |
| ASM-O-003 | User understands basic UI conventions | Usability | User testing |
| ASM-O-004 | Unit settings are persistent per profile | Personalization | Functional test |

### 3.2 Environmental Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-ENV-001 | Operating temperature: -40°C to +85°C | Automotive grade | Hardware spec |
| ASM-ENV-002 | Vibration within automotive specifications | Standard assumption | Platform test |
| ASM-ENV-003 | EMC compliance per automotive standards | Regulatory | Certification |

---

## 4. Development Assumptions

### 4.1 Process Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-D-001 | Developers follow secure coding guidelines | Quality | Training, review |
| ASM-D-002 | CI/CD pipeline catches regressions | Automation | Pipeline monitoring |
| ASM-D-003 | Static analysis runs on every PR | Quality gate | CI configuration |
| ASM-D-004 | Tests achieve required coverage | Verification | Coverage reports |

### 4.2 Toolchain Assumptions

| ID | Assumption | Rationale | Verification Method |
|----|------------|-----------|---------------------|
| ASM-T-001 | GCC 11+ or Clang 14+ is used | C++17 support | Build configuration |
| ASM-T-002 | CMake 3.21+ is available | Build system | Requirements |
| ASM-T-003 | Google Test is available for testing | Test framework | Build dependency |

---

## 5. Scope Limitations

### 5.1 Not Implemented in v1

| Item | Reason | Future Scope |
|------|--------|--------------|
| Plugin signature verification | Requires HSM | v2 |
| Encrypted IPC | Desktop dev focus | Production |
| External connectivity (WiFi, cellular) | Separate module | v2 |
| OTA updates | Separate project | Future |
| Multi-language UI | Localization phase | v1.1 |
| Voice control | Separate module | v2 |

### 5.2 Desktop Build Limitations

| Limitation | Production Approach |
|------------|---------------------|
| No hardware security module | HSM for key storage |
| Simulated signals | Real CAN/LIN interface |
| No secure boot | Platform secure boot |
| No process sandboxing | OS-level isolation |

---

## 6. Compliance Notes

### 6.1 ISO 26262 Tailoring

| Part | Tailoring | Rationale |
|------|-----------|-----------|
| Part 5 (Hardware) | Out of scope | Software deliverable only |
| Part 8.11 (Proven in use) | N/A | New development |
| Part 11 (Semiconductors) | Inherited | Platform responsibility |

### 6.2 ISO/SAE 21434 Tailoring

| Clause | Tailoring | Rationale |
|--------|-----------|-----------|
| Clause 10 (Production) | Partial | Desktop build focus |
| Clause 11 (Operations) | Scaffolded | Post-production scope |
| Clause 13 (Distributed) | N/A | Single organization |

---

## 7. Assumption Validation

### 7.1 Review Schedule

| Assumption Category | Review Frequency | Reviewer |
|---------------------|------------------|----------|
| Safety | Per release | Safety Engineer |
| Security | Per release | Security Engineer |
| Operational | Quarterly | Product Owner |
| Development | Per sprint | Tech Lead |

### 7.2 Assumption Changes

Changes to assumptions require:
1. Impact analysis on safety/security
2. Update to relevant documentation
3. Review and approval
4. Traceability update

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-23 | Team | Initial release |
