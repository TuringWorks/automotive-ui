# Traceability Template (Req → Design → Implementation → Test → Evidence)
Version: 0.1  
Date: 2025-12-23

This file is designed to be generated/updated by `tools/traceability/`.
Keep IDs stable. Use one row per requirement.

---

## 1) Conventions
- Requirements: `SYS-*`, `SR-CL-*`, `FR-INF-*`, `CR-INF-*`
- Design artifacts: `ARCH-*`, `SAFE-*`, `CS-*`
- Tests: `T-*`, `SEC-*`
- Evidence: build logs, test reports, review sign-offs

---

## 2) Traceability Table
| Requirement ID | Requirement Summary | Design Artifact(s) | Implementation File(s) | Test ID(s) | Evidence Artifact(s) | Status |
|---|---|---|---|---|---|---|
| SR-CL-001 | Speed updated ≥10Hz; stale <300ms | SAFE-VER-001 | `driver_ui/safety_core/...` | T-CL-001 | `reports/test/cluster.json` | Planned |
| SR-CL-002 | Clamp invalid; show “—” | SAFE-VER-001 | `shared/cpp/signal/...` | T-CL-002 | `reports/test/cluster.json` | Planned |
| CR-INF-002 | Permission gating for privileged settings | CS-TEST-001 | `infotainment_ui/app/...` | SEC-002 | `reports/test/security.json` | Planned |

---

## 3) Review/Approval Log (ASPICE-friendly)
| Date | Reviewer | Scope | Result | Notes |
|---|---|---|---|---|
| YYYY-MM-DD | Name | Req baseline | Pass/Fail | |
