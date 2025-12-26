# ADAS Safety Requirements Addendum (ISO 26262 + SOTIF)
Version: 0.2  
Date: 2025-12-25  
Applies to: **Driver UI (Cluster)** + ADAS/ADS-related UI in **Infotainment**

This addendum is designed to plug into:
- `Automotive_UI_LLM_Agents_Spec.md`
- `Safety_HARA_SOTIF_Skeleton.md`
- `ADAS_Autonomy_UI_Spec.md`
- `Traceability_Template.md`

> **Normative intent:** Provide **UI-facing** functional safety and SOTIF requirements that can be traced to hazards, tests, and evidence artifacts. Final ASIL targets and full safety case content depend on your item definition and HARA outcomes.

---

## 1) Safety Scope and Item Boundary (UI-facing)
### 1.1 Driver UI scope (safety-relevant)
- ADAS engagement/availability indication
- Takeover request / MRM messaging (L3+)
- Driver monitoring status (hands-on/attention) as **summary** only (no raw gaze processing in UI)
- Degraded mode indicators for sensor confidence/freshness

### 1.2 Infotainment scope (safety-supporting)
- Education, ODD viewer, post-drive summaries, event timelines
- Must not be the *only* source of any safety-critical message

### 1.3 Safety partition constraints (recap)
- No safety logic in QML/JS in Driver UI.
- Deterministic C++ safety core owns state machines, thresholds, validity, and gating.
- UI must never show “ENGAGED” when `availability=false` or when signals are invalid/stale.

---

## 2) Hazard Classes (UI-specific) — Starting Point
Use these as seeds in `Safety_HARA_SOTIF_Skeleton.md`.

### 2.1 HMI hazard patterns
- **HMI-HZ-001:** False “engaged” indication when ADAS/ADS is unavailable.
- **HMI-HZ-002:** Late/missed takeover request (L3+).
- **HMI-HZ-003:** Ambiguous responsibility owner (driver vs system).
- **HMI-HZ-004:** Stale/invalid perception shown as current.
- **HMI-HZ-005:** Alert occlusion by non-critical overlays.
- **HMI-HZ-006:** Overload (too many simultaneous alerts) → distraction.
- **HMI-HZ-007:** Misleading lane visualization when lane confidence is low.
- **HMI-HZ-008:** Infotainment shows autonomy state inconsistent with Driver UI (cross-screen inconsistency).

---

## 3) Safety Goals (SG) — UI-facing
> Map these to your HARA table. Assign ASIL targets later.

- **SG-ADAS-001:** Prevent misleading ADAS/ADS engagement and availability indications.
- **SG-ADAS-002:** Ensure takeover requests and MRM states are unmissable and timely.
- **SG-ADAS-003:** Always communicate responsibility owner (driver vs system) in a level-consistent way.
- **SG-ADAS-004:** Prevent stale/invalid signal presentation from appearing valid.
- **SG-ADAS-005:** Ensure critical alerts/telltales cannot be occluded by non-critical UI.
- **SG-ADAS-006:** Ensure degraded mode is clear and limits are communicated.

---

## 4) Functional Safety Requirements (FSR) — Driver UI
Requirement ID prefix: `SR-CL-ADAS-*` (safety) and `FR-CL-ADAS-*` (functional).

### 4.1 Engagement/Availability Integrity
- **SR-CL-ADAS-100:** The Driver UI shall never render an “ENGAGED” ADAS/ADS state unless `AdasEngagement.engaged=true` **and** `AdasEngagement.available=true` **and** message validity is true.
- **SR-CL-ADAS-101:** The Driver UI shall indicate “Unavailable” within **300 ms** when availability transitions to false (valid message).
- **SR-CL-ADAS-102:** If the ADAS/ADS engagement state is unknown (missing, stale, invalid), the Driver UI shall default to **OFF/UNAVAILABLE** indicators and suppress any “active” styling.

### 4.2 Timing and Freshness
- **SR-CL-ADAS-110:** Engagement state changes shall be reflected on-screen within **100 ms** of receipt of a valid update.
- **SR-CL-ADAS-111:** The UI shall indicate staleness for driver-critical ADAS/ADS signals (engagement, takeover, MRM, speed setpoint) within **≤300 ms** of missing expected updates.
- **SR-CL-ADAS-112:** The safety core shall apply monotonic time checks and reject non-monotonic timestamps to prevent replay/ordering artifacts from appearing current.

### 4.3 Takeover Requests (L3+)
- **SR-CL-ADAS-120:** A takeover request shall display a non-occludable banner and escalation cues until cleared by the safety core.
- **SR-CL-ADAS-121:** Takeover urgency levels shall map to deterministic, consistent HMI patterns (shape + text + optional audio/haptic hooks).
- **SR-CL-ADAS-122:** When takeover countdown reaches a configured threshold, the UI shall enter an “Immediate Action Required” presentation mode.

### 4.4 Minimal Risk Maneuver (MRM)
- **SR-CL-ADAS-130:** When `MRM_ACTIVE`, the Driver UI shall show a persistent state label and suppress non-critical overlays.
- **SR-CL-ADAS-131:** The Driver UI shall show “System performing safe stop” (or equivalent) and never prompt the user to take over if controls are disabled by design.

### 4.5 Degraded Mode and Perception Confidence
- **SR-CL-ADAS-140:** If lane/perception confidence drops below threshold, the Driver UI shall display DEGRADED within **≤300 ms** and modify lane visualization to avoid false precision.
- **SR-CL-ADAS-141:** Lane visualization shall reflect quality (e.g., solid/dashed/greyed) and shall not imply a centered corridor when lane boundaries are uncertain.
- **SR-CL-ADAS-142:** If object list validity is false, the UI shall hide object markers rather than show stale positions.

### 4.6 Alert Occlusion and Priority
- **SR-CL-ADAS-150:** Critical ADAS/ADS alerts (takeover/MRM/fault lockout) shall have priority over all other notifications and cannot be occluded.
- **SR-CL-ADAS-151:** The UI shall limit concurrent non-critical alerts to a configured maximum to reduce distraction.

### 4.7 Cross-Screen Consistency (Driver UI vs Infotainment)
- **SR-CL-ADAS-160:** Driver UI shall be the authoritative visual source for safety-critical ADAS/ADS state.
- **SR-CL-ADAS-161:** If infotainment shows autonomy state, it shall subscribe to the same `AdasEngagement` topic and display the same state (with any mismatch flagged as a fault in infotainment, not overridden).

---

## 5) SOTIF Requirements (SOTIF-UI-*)
Requirement ID prefix: `SOTIF-UI-*`.

- **SOTIF-UI-001:** The system shall provide explicit “why unavailable” explanations (e.g., lane lines poor, weather) without overstating capability.
- **SOTIF-UI-002:** The UI shall avoid reliance on color alone; all critical states must have redundant cues (icon + text + shape).
- **SOTIF-UI-003:** The UI shall provide a “capability boundary” explanation screen in infotainment that teaches the user the current level and responsibility owner.
- **SOTIF-UI-004:** The UI shall mitigate information overload by bundling related limitations and limiting simultaneous prompts.
- **SOTIF-UI-005:** The UI shall offer a “glare/night” mode policy (auto) with contrast and legibility maintained.

---

## 6) Verification Plan Addendum (Tests + Evidence)
> Add these to `docs/safety/verification_plan.md` and traceability.

### 6.1 Test IDs (suggested)
- **T-ADAS-001:** False engaged suppression (invalid/availability false → not engaged)
- **T-ADAS-002:** Engagement timing (≤100 ms UI update)
- **T-ADAS-003:** Staleness indicator (≤300 ms)
- **T-ADAS-004:** Takeover banner non-occludable + escalation sequence
- **T-ADAS-005:** MRM persistent state + overlay suppression
- **T-ADAS-006:** Degraded lane visualization rules
- **T-ADAS-007:** Cross-screen consistency check (infotainment matches driver state)

### 6.2 Evidence artifacts (recommended)
- `reports/test/adas_driver.json`
- `reports/test/adas_infotainment.json`
- `reports/metrics/ui_latency.csv`
- `reports/static/clang_tidy.txt`
- `reports/static/qmllint.txt`

---

## 7) Traceability Instructions
Add rows to `Traceability_Template.md` (or generated table) for each requirement:
- Requirement ID → Design artifact (`Safety_HARA_SOTIF_Skeleton.md`, `ADAS_Autonomy_UI_Spec.md`) → Implementation file(s) → Test ID(s) → Evidence artifact(s).

Example row:
| SR-CL-ADAS-120 | Takeover banner non-occludable | SAFE-VER-001 | driver_ui/qml/... | T-ADAS-004 | reports/test/adas_driver.json | Planned |

---

## 8) Eos ADAS-Inspired Safety/SOTIF Extensions (UI-focused)

The Eos ADAS demo emphasizes a high-fidelity 3D ADAS scene with rapid “preset” switching (lighting) and weather effects. When adopting these patterns, apply the following safety constraints:

### 8.1 Preset switching safety constraints
- **SR-CL-ADAS-170:** Environment preset switching (dawn/day/dusk/night) shall not block the UI thread and shall not delay safety-critical alert rendering (takeover/MRM/fault lockout).
- **SR-CL-ADAS-171:** During preset switching, the UI shall maintain the last known valid safety-critical state indicators (engagement, takeover) without flicker to an incorrect state.

### 8.2 Weather effects as SOTIF cues (not primary safety signals)
- **SOTIF-UI-010:** Weather particle effects (rain/snow/mist) shall be treated as informational only; the Driver UI shall still provide explicit text/icon degraded-mode cues derived from perception confidence/ODD status.
- **SOTIF-UI-011:** When heavy-weather conditions reduce perception confidence, the UI shall present a clear limitation reason (“Limited visibility / lane quality low”) and shall not imply lane precision.

### 8.3 Reflection/animation degradation rule
- **SR-CL-ADAS-172:** Reflection probes and skeletal animation updates shall be degraded or disabled before reducing any safety-critical HMI content fidelity.

### 8.4 Event-list replay (verification builds)
- **SR-CL-ADAS-173:** Scenario replay controls (event list simulator) shall be restricted to engineering/verification builds and shall never be available in customer production builds of Driver UI.
- **SR-CL-ADAS-174:** Event replay must be clearly labeled “Simulation” and must never be mixed with live vehicle data in the same session without a full reset.

### 8.5 Additional verification tests (add to V&V)
- **T-ADAS-008:** Environment preset switching does not delay takeover/MRM banners.
- **T-ADAS-009:** Weather effects toggling does not change safety state logic; degraded-mode remains correct.
- **T-ADAS-010:** Reflection/animation degradation keeps safety cues visible and stable.
