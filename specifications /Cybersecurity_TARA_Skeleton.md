# Cybersecurity Artifacts Skeleton (ISO/SAE 21434 + UNECE R155)
Version: 0.1  
Date: 2025-12-23

This file is a **starter skeleton** intended to be filled and iterated by the Cybersecurity Engineer agent.

---

## 1) Cybersecurity Plan (CS-PLAN-001)
### 1.1 Scope
- Infotainment UI (primary)
- Shared services (IPC, settings, logging)
- Driver UI: minimal surface area; treat as high-integrity consumer of signals

### 1.2 Roles & responsibilities
- Cybersecurity Manager:
- Security Engineer:
- Incident Response Owner:
- Tool Owner:

### 1.3 Security lifecycle and evidence (R155-aligned)
- CSMS linkage (process placeholders)
- Vulnerability intake and tracking mechanism
- Release security review checklist
- Monitoring and logging policy

---

## 2) Asset Inventory (TARA-INV-001)
| Asset | Location | Sensitivity | Notes |
|---|---|---|---|
| User profiles/settings | `UserSettingsService` | Medium | includes personalization |
| Connectivity tokens (future) | secure storage | High | must be protected |
| Plugin modules | infotainment plugins | High | integrity critical |
| Vehicle signal IPC | shared IPC | High | spoofing impacts safety perception |

---

## 3) Threat Scenario Catalog (TARA)
### 3.1 Threat model assumptions
- Desktop build is for development; production requires HW root-of-trust (future-ready).
- IPC boundaries must treat messages as untrusted unless authenticated.

### 3.2 Threat scenarios table
| TARA ID | Threat | Attack Vector | Impact | Likelihood | Risk | Security Goal ID |
|---|---|---|---|---|---|---|
| TARA-001 | Signal spoofing to UI | IPC injection | Misleads driver | TBD | TBD | CSG-001 |
| TARA-002 | Unauthorized settings change | privilege escalation | Safety / privacy | TBD | TBD | CSG-002 |
| TARA-003 | Malicious plugin | tampered module | RCE / data theft | TBD | TBD | CSG-003 |
| TARA-004 | Log exfiltration | insecure logs | PII leakage | TBD | TBD | CSG-004 |

### 3.3 Cybersecurity goals (examples)
- **CSG-001:** Ensure authenticity/integrity of vehicle-originated signals presented to UIs.
- **CSG-002:** Enforce permission model for sensitive settings and operations.
- **CSG-003:** Ensure software update/plugin integrity and sandboxing.
- **CSG-004:** Prevent leakage of sensitive data via logs and telemetry.

---

## 4) Security Requirements (CR-INF-* examples)
- **CR-INF-001:** IPC messages shall include versioning and integrity checks; reject malformed messages.
- **CR-INF-002:** Privileged actions shall require explicit permission checks and audit events.
- **CR-INF-003:** Plugins shall be loaded only from signed manifests (scaffold in v1).
- **CR-INF-004:** Logs shall redact PII by default; debug logging gated by build flag.

---

## 5) Security Architecture Notes
### 5.1 IPC hardening
- Schema-based encoding (protobuf/flatbuffers/jsonschema)
- Message authentication (future: MAC/signature)
- Replay protection (nonce/time window) — scaffold

### 5.2 Permission model
- Capabilities assigned per module (media, phone, vehicle settings)
- User consent flows (where applicable)
- Audit events emitted for privileged actions

### 5.3 Plugin integrity and sandboxing
- Signed plugin manifest (future-ready)
- Separate process for untrusted plugins (future)
- Restrict filesystem/network access (desktop: best-effort)

---

## 6) Security Test Plan (CS-TEST-001)
| Test ID | Requirement | Method | Steps | Pass Criteria |
|---|---|---|---|---|
| SEC-001 | CR-INF-001 | Unit | inject malformed IPC frames | rejected + logged (no crash) |
| SEC-002 | CR-INF-002 | Unit/IT | attempt privileged call without capability | denied + audit event |
| SEC-003 | CR-INF-004 | Unit | log PII string | redacted output |

---

## 7) Incident Response Hooks (R155-ready)
- Vulnerability reporting channel: TBD
- Severity rubric: TBD
- Patch release process: TBD
- Forensics logging guidelines: TBD
