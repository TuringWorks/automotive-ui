# ADAS Cybersecurity Addendum (ISO/SAE 21434 + UNECE WP.29 R155)
Version: 0.1  
Date: 2025-12-25  
Applies to: ADAS/ADS UI surfaces in **Driver UI** + **Infotainment UI**

This addendum is designed to plug into:
- `Cybersecurity_TARA_Skeleton.md`
- `ADAS_Autonomy_UI_Spec.md`
- `Automotive_UI_LLM_Agents_Spec.md`
- `Traceability_Template.md`

---

## 1) Security Scope (ADAS UI surfaces)
### 1.1 Assets (UI-relevant)
- ADAS/ADS engagement state (`AdasEngagement`)
- Takeover requests + countdown (`TakeoverRequest`)
- ODD status and reasons (`OddStatus`)
- Event logs / disengagement reasons (`HmiEventLog`)
- Engineering / perception explorer screens (infotainment only, restricted)
- Plugin and content surfaces in infotainment

### 1.2 Threat Themes (UI-facing)
- **Spoofed autonomy state** to mislead driver (integrity).
- **Replay attacks** of engagement/takeover messages (freshness).
- **Unauthorized enabling/disabling** of autonomy features (authorization).
- **Tampering with event logs** to hide faults (auditability).
- **Diagnostics/engineering mode exposure** to untrusted users (privilege escalation).
- **Denial-of-service** causing UI lag or missed takeover prompts (availability).

---

## 2) Cybersecurity Goals (CSG) — ADAS UI-facing
- **CSG-ADAS-001:** Ensure authenticity/integrity and anti-replay properties for ADAS/ADS state messages presented to the driver.
- **CSG-ADAS-002:** Ensure privileged autonomy controls require authorization and leave an auditable trail.
- **CSG-ADAS-003:** Ensure diagnostic/engineering screens are access-controlled and motion-locked.
- **CSG-ADAS-004:** Ensure infotainment plugin/content cannot inject false ADAS states into Driver UI.
- **CSG-ADAS-005:** Maintain availability of takeover/MRM messaging under adverse conditions.

---

## 3) Security Requirements (CR-ADAS-*)
Requirement ID prefix: `CR-ADAS-*` (shared) and `CR-INF-ADAS-*` (infotainment).

### 3.1 Message Integrity, Freshness, Anti-Replay
- **CR-ADAS-100:** ADAS/ADS IPC messages shall be schema-validated and versioned; malformed messages rejected with rate-limited logging.
- **CR-ADAS-101:** All driver-critical ADAS/ADS messages shall include: `seq`, monotonic timestamp, and freshness window enforcement in the receiver.
- **CR-ADAS-102:** The receiver shall reject messages that are outside a configurable freshness window or that regress `seq` (anti-replay / anti-reorder best-effort).
- **CR-ADAS-103:** Driver UI shall treat the message bus as untrusted: if integrity checks fail, default to OFF/UNAVAILABLE and raise a security fault indicator (non-occludable).

### 3.2 Authorization and Privileged Actions
- **CR-INF-ADAS-110:** Enabling/disabling autonomy features, changing core parameters (following gap defaults, steering assistance strength), or entering engineering views shall require explicit authorization.
- **CR-INF-ADAS-111:** Privileged actions shall generate audit events (who/what/when) to a tamper-evident log store (scaffold v1).
- **CR-INF-ADAS-112:** Any remote-assistance connectivity control shall require explicit user consent and show connection status persistently while active.

### 3.3 Diagnostic/Engineering Mode Protection
- **CR-INF-ADAS-120:** Engineering screens (Perception Explorer) shall be locked behind an authorization gate and disabled when vehicle is in motion (policy-driven).
- **CR-INF-ADAS-121:** Diagnostic exports (logs, screenshots) shall be access-controlled and redact PII by default.
- **CR-INF-ADAS-122:** Debug toggles shall be compile-time gated for production builds.

### 3.4 Isolation Between Infotainment and Driver UI
- **CR-ADAS-130:** Infotainment shall not have write access to Driver UI ADAS state channels.
- **CR-ADAS-131:** Any cross-process message from infotainment to driver UI shall be limited to non-safety, non-ADAS data (theme/language) unless explicitly safety-reviewed.
- **CR-ADAS-132:** If an infotainment plugin attempts to access ADAS state beyond its capability set, access shall be denied and audited.

### 3.5 Availability / DoS Resilience
- **CR-ADAS-140:** Driver UI shall prioritize processing and rendering of takeover/MRM messages over non-critical data.
- **CR-ADAS-141:** Rate-limit non-critical IPC to prevent UI starvation.
- **CR-ADAS-142:** Implement a watchdog metric for UI latency; on threshold breach, degrade visuals (quality manager) while preserving critical cues.

---

## 4) Security Test Plan Addendum (CS-TEST-ADAS-*)
### 4.1 Test IDs (suggested)
- **SEC-ADAS-001:** Malformed IPC frame rejection (no crash)
- **SEC-ADAS-002:** Replay/seq regression rejection → OFF/UNAVAILABLE fallback
- **SEC-ADAS-003:** Freshness window expiry → stale indicator + safe fallback
- **SEC-ADAS-004:** Unauthorized privileged action denied + audit event
- **SEC-ADAS-005:** Engineering screen motion lockout
- **SEC-ADAS-006:** Plugin capability enforcement
- **SEC-ADAS-007:** DoS rate-limiting preserves takeover rendering

### 4.2 Evidence artifacts (recommended)
- `reports/test/security_adas.json`
- `reports/audit/audit_log.ndjson`
- `reports/metrics/ipc_rate_limits.csv`
- `reports/metrics/ui_watchdog.csv`

---

## 5) Traceability Instructions
Add rows to `Traceability_Template.md` (or generated table) for each CR-ADAS/CR-INF-ADAS requirement:
- Requirement ID → Design artifact (`Cybersecurity_TARA_Skeleton.md`, `ADAS_Autonomy_UI_Spec.md`) → Implementation → Tests → Evidence.

Example row:
| CR-ADAS-102 | Reject stale/replayed messages | CS-PLAN-001 | shared/cpp/ipc/... | SEC-ADAS-002 | reports/test/security_adas.json | Planned |
