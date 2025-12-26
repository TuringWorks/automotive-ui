# ADAS & Autonomy UI Specification (Qt 6 + QML + Qt Quick 3D)
Version: 1.0  
Date: 2025-12-25  
Applies to: **Driver UI (Cluster)** + **Infotainment UI** subprojects

This specification extends the existing Automotive UI suite spec and defines **ADAS + SAE Levels of Driving Automation (L1–L5)** UI capabilities, requirements, data contracts, and safety/cyber constraints.

**Design inspiration:** high-end Qt Quick 3D ADAS demos using **particle weather effects** and **realtime reflections**, and a configurable 3D benchmark-style approach that exposes performance/quality knobs (lights, model complexity, textures, AA).

---

## 0) Normative References (for taxonomy + UI constraints)
- SAE J3016 taxonomy for Levels 0–5 (use as definition baseline).  
- NHTSA “Levels of Automation” overview (UI text and user education materials may use this phrasing).  
- ISO 15008: minimum requirements for legibility of in-vehicle visual displays with dynamic information.  
- UNECE R79 references “Automatically Commanded Steering Function (ACSF)” (useful alignment for lane-keeping/lane-centering features).  

---

## 1) Scope and Principles

### 1.1 Scope
Implement UI/HMI for:
- **ADAS (L1–L2):** ACC, LKA/LKAS, LCA/lane-centering, AEB status, blind spot monitoring visualization, driver monitoring status.
- **ADS (L3–L5):** Operational Design Domain (ODD) presentation, engagement/availability states, takeover requests, minimal risk maneuver (MRM) state, route/mission constraints, passenger experience modes.

### 1.2 Partitioning and Safety Rules (mandatory)
- Driver UI and Infotainment are separate processes (already mandated).
- **Driver UI safety rules remain unchanged:** no safety-critical logic in QML/JS; all ADAS state evaluation and safety gating in deterministic C++.
- Infotainment may show richer info, but **must not be the sole source** of any safety-critical ADAS status; Driver UI remains authoritative for safety messaging.

### 1.3 HMI Tenets
- **Glanceability first** for safety-relevant ADAS status (Driver UI).
- **No ambiguous autonomy state**: the system must always convey (a) availability, (b) engagement, (c) who is responsible for DDT / fallback. SAE and NHTSA emphasize level-specific expectations.
- **Legibility and contrast** must meet in-vehicle display minimums (ISO 15008).
- **Degraded mode** is explicit: sensor uncertainty, camera blockage, poor lane lines, bad weather → must change the UI state, not just reduce performance.

---

## 2) Feature Map by Automation Level

### 2.1 Canonical Level Responsibilities (UI must reflect this)
| Level | Dynamic Driving Task (DDT) responsibility | “Takeover” / fallback expectation |
|---|---|---|
| L1 | Driver performs DDT; system assists in **either** steering or accel/brake | Driver is primary; system can disengage anytime |
| L2 | Driver performs DDT supervision; system assists in **both** steering and accel/brake | Driver must continuously supervise |
| L3 | System performs DDT in ODD; user must be **fallback-ready** | System requests takeover when needed |
| L4 | System performs DDT and fallback in ODD | May not require takeover within ODD |
| L5 | System performs DDT and fallback everywhere | No driver role required |

> UI copy must not overpromise beyond the actual level. The HMI must continuously show the “responsibility owner.”

### 2.2 Level-driven UI Capabilities (summary)
- **L1–L2 (ADAS):** lane visualization + lead vehicle + speed setpoint + hands-on / attention status + ADAS limits.
- **L3 (Conditional):** explicit ODD gate + takeover pipeline + countdown + MRM state + user readiness prompts.
- **L4–L5:** mission/ODD status + passenger experience mode + safe stop + remote assistance hooks (if applicable).

---

## 3) Shared ADAS/ADS Architecture (Both Apps)

### 3.1 Services and Models (C++ authoritative)
Create these logical services (can be separate libs):
- `AdasStateService` (authoritative engagement + availability + faults)
- `OdsService` / `OddService` (ODD boundaries: road type, weather limit, speed limit, geo-fence)
- `PerceptionModel` (objects, lanes, free space, drivable corridor)
- `DriverMonitoringService` (attention/hands-on/eye-gaze status summary for UI)
- `TakeoverManager` (L3+ request-to-intervene state machine)
- `HmiEventLog` (auditable timeline for safety + cyber)

**IPC schema-based messages** must be versioned and validated (existing security requirements apply).

### 3.2 Data Contracts (minimum set)
All signals include: `timestamp`, `validity`, `confidence`, `source`, `seq`, `units`.

**Core topics**
- `VehicleKinematics`: speed, yaw rate, accel, steering angle
- `LaneModel`: left/right boundaries, quality score, lane type
- `ObjectList`: dynamic objects (vehicles, pedestrians, cyclists, cones)
- `LeadVehicle`: id, relative distance, relative speed
- `DrivableCorridor`: planned path corridor + curvature
- `AdasEngagement`: mode, availability, set speed, following gap, limits
- `OddStatus`: inside/outside ODD + reasons
- `TakeoverRequest`: active, urgency, countdown, required action
- `DmsStatus`: hands-on, attention, distraction state

### 3.3 Visualization Pipeline (Qt Quick 3D)
Use Qt Quick 3D as the 3D rendering layer: it supports mixing 2D Qt Quick content with 3D content via the scene graph.

Recommended 3D primitives:
- **Road ribbon** + lane lines (procedural mesh or instanced segments)
- **Instanced objects** for vehicles/cones/signs to reduce draw calls (Qt Quick 3D instancing).
- **Weather particles** (rain/snow/mist) for environmental feedback (Particles3D).
- **Realtime reflections** for realism via ReflectionProbe (use strategically; quality-scaled).

### 3.4 “Benchmark-inspired” Quality Manager (mandatory)
Implement `AdasVisualQualityManager` inspired by the benchmark demo’s approach of adjusting scene and render parameters such as lights, textures, materials, AA, and scene complexity.

**Inputs**
- FPS, GPU time, CPU time, thermal flags, memory pressure, weather particle count, object count

**Outputs (examples)**
- Dynamic resolution scale (0.7–1.0)
- MSAA level
- ReflectionProbe update rate (e.g., once vs periodic)
- Particle budget (emit rate, max particles)
- Instancing mode (on always; cap unique materials)
- LOD for meshes (high/med/low)
- Post effects toggles (bloom/fog)

**Safety rule:** quality reduction must **not** remove safety-critical cues; instead it reduces aesthetics first (reflections, extra effects), then reduces non-critical detail density.

---

## 4) Driver UI (Cluster) — ADAS/Autonomy Spec

### 4.1 Driver UI ADAS Zones (layout contract)
- **Primary Gauge Zone:** speed, speed limit, set speed (if ACC/L2), critical alerts
- **ADAS State Zone:** lane-centering status, following distance, lead vehicle
- **Perception Mini-View:** stylized lane + lead vehicle + adjacent objects (2D + optional 3D)
- **Takeover / MRM Banner:** full-width, cannot be occluded

### 4.2 HMI State Machine (authoritative)
States (minimum):
1. `ADAS_OFF`
2. `ADAS_AVAILABLE`
3. `ADAS_ENGAGED_L1`
4. `ADAS_ENGAGED_L2`
5. `ADS_AVAILABLE_L3`
6. `ADS_ENGAGED_L3`
7. `TAKEOVER_REQUESTED` (with urgency levels)
8. `MRM_ACTIVE`
9. `DEGRADED` (sensor confidence low)
10. `FAULT_LOCKOUT` (requires service/user action)

Each state defines:
- allowed overlays
- required persistent telltales
- audio/haptic hooks (stub on desktop)
- color/shape semantics (avoid relying on color alone)

### 4.3 Driver UI Requirements (examples; extend as needed)
**Functional (FR-CL-ADAS-*)**
- FR-CL-ADAS-001: Display ADAS availability vs engagement distinctly (text + icon).
- FR-CL-ADAS-002: Show lane quality and confidence (e.g., solid vs dashed, explicit “limited” label).
- FR-CL-ADAS-003: When L2 engaged, show continuous supervision requirement (attention prompt rules).

**Safety (SR-CL-ADAS-*)**
- SR-CL-ADAS-010: Any change in engagement state must be reflected within 100 ms of state update receipt (valid message).
- SR-CL-ADAS-011: If perception confidence drops below threshold, show DEGRADED within 300 ms and constrain state to safe fallback.
- SR-CL-ADAS-012: Takeover requests must be unmissable: banner + escalating cues; cannot be dismissed until cleared.
- SR-CL-ADAS-013: If signals stale/invalid, show “—” or explicit “Unavailable” and prevent misleading green/active states.

### 4.4 L1 UI (Driver Assistance)
Examples: ACC **or** LKAS.
- Show active function(s) as “Assist” not “Autopilot”.
- Show driver remains responsible always.
- UI must show limitation reasons: “Lane lines not detected”, “Radar blocked”, etc.

### 4.5 L2 UI (Partial Automation)
Examples: lane-centering + ACC.
- Add **hands-on / attention** indicator and reminders.
- Provide a clear “Engaged” affordance and a clear disengage indicator.
- Perception mini-view shows lead vehicle and lane corridor; include confidence.

### 4.6 L3 UI (Conditional Automation)
- Show **ODD gate** prominently: “Available only on divided highways” etc.
- Provide **Takeover Pipeline**:
  - pre-warning (“ODD ending soon”)
  - request-to-intervene (countdown)
  - escalation (audible/haptic)
  - MRM active if no response (system-controlled)
- UI language: refer to the user as “take over when requested” consistent with L3 framing.

### 4.7 L4 UI (High Automation in ODD)
- Primary driver UI shifts from “supervise” to “system driving within ODD” while still showing:
  - ODD boundaries and current status
  - what the system will do if ODD is violated (safe stop)
- If steering wheel exists, clarify whether human controls are enabled/locked.

### 4.8 L5 UI (Full Automation)
- Driver UI becomes “vehicle status panel” (speed, route progress, system health).
- Remove driver-supervision prompts; focus on passenger information and safety alerts.
- If a steering wheel exists (legacy platform), provide a safe/manual override state.

---

## 5) Infotainment UI — ADAS/Autonomy Spec

### 5.1 Core Screens
- **Autonomy Center (new top-level)**
  - Capability overview (what’s installed + allowed)
  - ODD viewer (maps, weather constraints, speed constraints)
  - Driver monitoring status (informational)
  - Event timeline (why disengaged, limitations, recent faults)
- **Trip Planner**
  - For L3–L5: routes aligned to ODD + charging + safe stop locations
- **Perception Explorer (engineering mode)**
  - Must be behind a protected toggle and disabled while driving (policy-driven)
- **Settings**
  - Feature toggles, calibration status, privacy controls

### 5.2 Infotainment Requirements (examples)
**Functional (FR-INF-ADAS-*)**
- FR-INF-ADAS-001: Provide a clear explanation of current automation level and user responsibility (education view referencing SAE/NHTSA taxonomy).
- FR-INF-ADAS-002: Provide ODD boundary visualization and “why unavailable” explanations.
- FR-INF-ADAS-003: Provide post-drive summaries (disengagement causes, environmental factors).

**Cyber (CR-INF-ADAS-*)**
- CR-INF-ADAS-010: Autonomy logs/events must be tamper-evident and access-controlled.
- CR-INF-ADAS-011: Any remote-assistance or connectivity UI must clearly show connection state and require explicit user consent.
- CR-INF-ADAS-012: Engineering/diagnostic screens must require authorization and be locked out in motion.

### 5.3 3D “Hero View” (optional, best-in-class)
Provide an optional high-fidelity 3D scene in Infotainment:
- Use instancing for object lists to scale to dense scenes
- Use weather particles to communicate environmental constraints
- Use reflection probes selectively for realism
- Must degrade gracefully using `AdasVisualQualityManager`.

---

## 6) UX Content and Copy Rules (Avoid Misrepresentation)
- Never claim “Full Self-Driving” unless it is actually L4/L5 under SAE definitions and within ODD where applicable.
- Use explicit verbs:
  - L1/L2: “Assist”, “Support”, “Keeps speed”, “Keeps lane” (driver supervises)
  - L3: “System drives; be ready to take over when requested”
  - L4/L5: “System drives” + ODD constraints (if L4)

---

## 7) Performance Budgets and Adaptation
- Target 60 fps render for 3D views; prioritize safety-critical cues over visual fidelity.
- Quality manager must guarantee:
  - consistent frame pacing for Driver UI
  - no dropped critical alert banners
  - perception visualization remains readable even at reduced fidelity

---

## 8) Verification & Validation (V&V)

### 8.1 Scenario Playback Harness (mandatory)
- Provide a “Scenario Runner” that replays:
  - lane dropouts
  - lead vehicle cut-in
  - heavy rain/snow (particles)
  - reflection-heavy night scenes
  - ODD boundary crossing
  - takeover request + no response → MRM

### 8.2 KPI Metrics
- HMI latency: state-change → UI update (target ≤100 ms for engagement state)
- Staleness detection latency: ≤300 ms (driver-critical signals)
- FPS and frame variance under stress scenes
- Correctness: no false “engaged” state when unavailable

### 8.3 Traceability
All ADAS/ADS requirements must be added to the traceability table and linked to tests and evidence artifacts.

---

## 9) Deliverables (Additions to Repo)
Create these docs:
- `docs/adas/overview.md` (this spec)
- `docs/adas/levels_matrix.md` (L1–L5 responsibilities + UI mapping)
- `docs/adas/driver_ui_requirements.md`
- `docs/adas/infotainment_requirements.md`
- `docs/adas/scenarios.md` (ADAS + SOTIF scenarios)
- `docs/adas/security.md` (TARA addendum)

And code modules:
- `shared/cpp/adas/*` (state machine, models)
- `shared/qml/AdasComponents/*` (icons, mini-view components)
- `tools/sim/scenarios/adas/*` (replay files)
---

## Sources used for design inspiration and taxonomy
- Qt public demo “Eos ADAS” (project description): https://git.qt.io/public-demos/eos-adas
- Qt blog: “Introducing Qt Quick 3D Benchmarking Application” (performance knobs inspiration): https://www.qt.io/blog/introducing-qtquick3d-benchmarking-application
- Qt Quick 3D overview: https://doc.qt.io/qt-6/qtquick3d-index.html
- Qt Quick 3D Instancing docs: https://doc.qt.io/qt-6/quick3d-instancing.html and https://doc.qt.io/qt-6/qml-qtquick3d-instancing.html
- Qt Quick 3D ReflectionProbe docs + example: https://doc.qt.io/qt-6/qml-qtquick3d-reflectionprobe.html and https://doc.qt.io/qt-6/qtquick3d-reflectionprobes-example.html
- Qt Quick 3D Particles3D example: https://doc.qt.io/qt-6/qtquick3d-particles3d-example.html
- NHTSA “Levels of Automation” PDF: https://www.nhtsa.gov/document/levels-automation
- SAE J3016 standard landing page: https://www.sae.org/standards/j3016-taxonomy-definitions-terms-related-driving-automation-systems-road-motor-vehicles
- ISO 15008 landing page: https://www.iso.org/standard/62784.html
- UNECE R79 PDF: https://unece.org/sites/default/files/2024-04/R079r5e.pdf
