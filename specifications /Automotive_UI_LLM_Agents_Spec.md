# Automotive UI Suite (Qt/QML) — LLM Agent Coding Specification
Version: 1.0  
Date: 2025-12-23  
Scope: Two subprojects (Driver UI + Infotainment UI) built with **Qt 6 + QML**, designed for **multi-platform** execution (Linux / Windows / macOS) and engineered to be **automotive-standard-ready** (ISO 26262, SOTIF, ASPICE, ISO/SAE 21434, UNECE R155, MISRA, AUTOSAR-aligned).

---

## 1) Purpose and Outcomes

### 1.1 Goal
Create a repo that can be developed largely by coding LLM agents, producing two applications:

1) **Driver UI (Cluster)**  
   Gauges, telltales, critical alerts, drive state, minimal secondary controls. Safety-relevant.

2) **Infotainment UI**  
   Media, navigation shell, phone, vehicle settings, apps, voice UI, multi-pane UX. Typically non-safety, but cybersecurity-critical.

Both UIs must feel **best-in-class** (clarity, responsiveness, animations, information hierarchy) while remaining **strictly safe and compliant** for automotive use.

### 1.2 Non-goals (v1)
- Shipping to a specific OEM ECU or certified production platform (we will design for it).
- Implementing real CAN stack or real vehicle actuation.
- Providing final ISO 26262 certification artifacts (we produce the structure + evidence-ready outputs).

---

## 2) Compliance Targets and Engineering Constraints

### 2.1 Standards / Guidance (mapped to repo artifacts)
| Standard / Guideline | Primary Concern | Required Repo Artifacts |
|---|---|---|
| ISO 26262 | Functional safety lifecycle, ASIL rigor | Safety plan, HARA, FSC/TSC stubs, safety requirements, traceability, verification strategy |
| ISO/PAS 21448 (SOTIF) | Intended functionality risks | SOTIF scenarios, performance limitations, misuse & environmental scenarios |
| Automotive SPICE (ASPICE) | Process maturity | Work products: requirements, architecture, design, unit tests, reviews, CM |
| ISO/SAE 21434 | Cybersecurity engineering | CS plan, TARA, security requirements, security testing |
| UNECE WP.29 R155 | Cybersecurity management | Evidence mapping: processes, monitoring hooks, incident response placeholders |
| MISRA C/C++ | Safer C/C++ | MISRA-oriented ruleset for C++ core, static analysis gates |
| AUTOSAR (aligned) | Modular ECU architecture | Interface boundaries, service-oriented approach, adapters for comm |

### 2.2 Safety Partitioning Rules (mandatory)
- **Driver UI** and **Infotainment UI** run as **separate processes**.
- **No safety-critical computation in QML/JS** (Driver UI). QML is rendering + bindings only.
- Driver UI backend is a **restricted, deterministic C++ core**:
  - no dynamic plugin loading in safety core
  - bounded memory allocations in steady-state
  - fixed update rates, validated signal ranges
- Infotainment may use richer patterns (plugins, web content), but must be sandboxed.

### 2.3 Quality Gates (minimum)
- Build must be reproducible via CI on Linux and at least one of Windows/macOS.
- Static analysis + lint gates required for every merge:
  - `clang-tidy` + rule profile (MISRA-inspired) on C++
  - `qmllint` on QML
  - formatting gates (`clang-format`, `qmlformat`)
- Unit tests + UI smoke tests required for both subprojects.
- Requirement-to-test traceability produced automatically.

---

## 3) System Architecture Overview

### 3.1 High-level Components
- **Shared Platform Layer (`shared/`)**
  - Signal model + validation
  - Timebase + scheduler (deterministic tick)
  - UI design system (QML components, tokens, typography, animations)
  - IPC interfaces + message schemas
  - Logging + metrics
  - Simulation harness (desktop)

- **Driver UI (`driver_ui/`)**
  - QML presentation layer (read-only rendering)
  - C++ safety core (signal ingestion, validation, state machine, alert prioritization)
  - Safety monitor (watchdogs, plausibility, stale data detection)
  - Degraded mode rendering

- **Infotainment UI (`infotainment_ui/`)**
  - QML app shell (home, nav shell, media, phone, settings)
  - App framework (modules/plugins)
  - Secure storage + permissions layer
  - Optional sandbox runner for untrusted content (future-ready)

### 3.2 Data Flow (desktop + vehicle-ready)
- `SignalSource` (simulator / adapters) → `SignalHub` → (validated) → app-specific state models → QML.
- IPC between apps is **explicit and minimal** (e.g., time, theme, language, vehicle profile).
- No cross-process shared memory unless required and safety-reviewed.

### 3.3 AUTOSAR-aligned interface boundary
Define a **Service Interface** boundary in `shared/interfaces/`:
- `VehicleSignalsService`
- `AlertsService`
- `UserSettingsService`

Provide adapters:
- `adapters/sim/` (JSON/protobuf file, replay)
- `adapters/socketcan/` (Linux-only placeholder)
- `adapters/mock/` (unit tests)

---

## 4) UX/HMI Principles (Best-in-class + Safe)

### 4.1 Driver UI HMI Principles
- **Glanceability:** key info readable within 200–300 ms.
- **Minimal cognitive load:** stable layout; motion is functional, not decorative.
- **Alert discipline:** strict prioritization, limited simultaneous alerts, clear actions.
- **Degraded behavior:** show stale/invalid signals clearly; avoid false precision.

### 4.2 Infotainment HMI Principles
- **Speed:** <16ms frame budget target at 60fps for core views.
- **Consistency:** component library + typography scale + spacing tokens.
- **Task-first:** reduce taps; persistent “Now Playing” and nav context.
- **Driver distraction mitigation (design):** lockouts for distracting features while moving (policy-driven).

---

## 5) Subproject Specifications

## 5.1 Driver UI (Cluster)

### 5.1.1 Primary Screens
- **Drive View (default)**
  - Speed (primary)
  - Gear state (PRNDL / mode)
  - Power/regen gauge or tach (configurable)
  - Battery / fuel range
  - Turn signals, high beam, telltales
  - ADAS status strip (if enabled)
- **Alerts & Warnings Overlay**
  - Priority ladder (P0..P3)
  - Audible cues (stubbed in desktop)
  - Driver confirmation flows (non-distracting)
- **Trip / Efficiency View**
  - Minimal summary; no deep graphs while moving
- **Settings (cluster-limited)**
  - Brightness, units, language, theme

### 5.1.2 Safety Requirements (examples; expanded in `docs/safety/requirements.md`)
- **SR-CL-001:** Speed display shall be updated at ≥10Hz and shall indicate stale data within 300ms of missed updates.
- **SR-CL-002:** Invalid signal ranges shall be clamped and flagged; UI shall show “—” when validity is false.
- **SR-CL-003:** Critical telltales shall remain visible and cannot be occluded by non-critical overlays.
- **SR-CL-004:** Degraded mode shall activate on loss of signal integrity, and the UI shall reduce to essential gauges.

### 5.1.3 Determinism Rules
- Fixed tick loop (e.g., 60Hz render + 20Hz signal state updates).
- No unbounded allocations after initialization in safety core.
- All signal conversions and formatting done in C++ core (not QML JS).

### 5.1.4 Driver UI Tech Stack
- Qt 6.x (LTS preferred), QML (Qt Quick)
- C++17/20 (choose one and lock it), no exceptions in safety core (policy)
- `QAbstractListModel` / `QObject` view models with explicit validity

---

## 5.2 Infotainment UI

### 5.2.1 Core Apps / Sections
- **Home**
  - Multi-tile layout: Nav preview, Media, Phone, Quick Controls
- **Media**
  - Sources: Bluetooth, USB, streaming mock
  - Queue, library browsing, now playing
- **Navigation Shell (v1 stub)**
  - Map surface placeholder + route guidance UI scaffolding
- **Phone**
  - Contacts, dialer, call UI (mock)
- **Vehicle**
  - Settings: HVAC (UI-only), drive modes (mock), lighting (mock)
- **App Drawer**
  - Modular “apps” loaded from local modules

### 5.2.2 Cybersecurity Requirements (examples; expanded in `docs/security/requirements.md`)
- **CR-INF-001:** All external content sources must be authenticated and integrity-checked (mocked in v1).
- **CR-INF-002:** Privileged vehicle settings require explicit permission gating.
- **CR-INF-003:** Logging shall avoid sensitive data by default; PII redaction policy required.

### 5.2.3 Infotainment Tech Stack
- Qt 6 + QML
- C++ services
- Plugin framework allowed **outside** safety-relevant boundaries
- Secure settings storage abstraction (per-OS backend)

---

## 6) Repository Layout (Required)
```
repo/
  README.md
  CMakeLists.txt
  cmake/
  shared/
    qml/
      DesignSystem/
      Components/
      Tokens/
    cpp/
      signal/
      ipc/
      logging/
      sched/
    interfaces/
      proto/ (or jsonschema/)
    tests/
  driver_ui/
    app/
      main.cpp
      qml/
      cpp/
    safety_core/
      cpp/
      tests/
    docs/
  infotainment_ui/
    app/
      main.cpp
      qml/
      cpp/
    plugins/
    docs/
  tools/
    sim/
      signal_replay/
      scenario_runner/
    traceability/
  docs/
    architecture/
    safety/
    security/
    process/
  .github/workflows/ (or equivalent CI)
```

---

## 7) Engineering Process (ASPICE-aligned Work Products)

### 7.1 Required Work Products
- **System Requirements**: `docs/requirements/system.md`
- **Software Requirements**:
  - `driver_ui/docs/requirements.md`
  - `infotainment_ui/docs/requirements.md`
- **Architecture**: `docs/architecture/overview.md`
- **Safety**:
  - `docs/safety/safety_plan.md`
  - `docs/safety/hara.md`
  - `docs/safety/safety_requirements.md`
  - `docs/safety/verification_plan.md`
- **Security**:
  - `docs/security/cybersecurity_plan.md`
  - `docs/security/tara.md`
  - `docs/security/security_requirements.md`
  - `docs/security/security_test_plan.md`
- **Traceability**:
  - `tools/traceability/` generates Req → Design → Test mapping

### 7.2 Definition of Done (DoD)
A feature is “Done” only if:
- Requirement updated (or added) with unique ID.
- Implementation merged with lint + static analysis passing.
- Unit tests updated + passing.
- Evidence updated: traceability entry + short verification note.

---

## 8) Testing & Verification Strategy

### 8.1 Test Types
- **Unit Tests (C++)**: signal validation, alert prioritization, state machines
- **QML Tests**: qmltestrunner for component behavior
- **UI Smoke Tests**: launch + screenshot comparisons (optional in v1)
- **Fault Injection (Driver UI)**:
  - stale signals
  - out-of-range values
  - jittery updates
  - missing frames
- **Security Tests (Infotainment)**:
  - permission gating tests
  - config tamper tests (mock)
  - plugin sandbox boundary tests (scaffold)

### 8.2 Coverage Targets (v1)
- Safety core: high coverage target (e.g., ≥80% lines; adjust by policy)
- UI layer: smoke + behavior tests focus, not line coverage

---

## 9) Build, Packaging, and Multi-platform

### 9.1 Build System
- CMake + Qt6
- Single top-level configure with options:
  - `-DBUILD_DRIVER_UI=ON/OFF`
  - `-DBUILD_INFOTAINMENT_UI=ON/OFF`
  - `-DENABLE_SIM=ON/OFF`

### 9.2 Packaging Targets
- Linux: AppImage or .deb (at least one)
- Windows: MSI (or zip bundle for v1)
- macOS: .app bundle (or dmg later)
- Provide `tools/run_desktop.sh|ps1` scripts for rapid launch.

---

## 10) LLM Coding Agent System Specification

## 10.1 Agent System Overview
LLM agents collaborate to produce code + documentation with strict controls:
- Agents operate on **small, reviewable changes**
- Agents must create/update:
  - requirements (IDs)
  - implementation
  - tests
  - traceability links

### 10.2 Shared Agent Rules (Hard Constraints)
- Never introduce safety-relevant logic into QML/JS for Driver UI.
- Never bypass quality gates (lint/static analysis/tests).
- Every PR must include:
  - summary
  - impacted requirement IDs
  - verification evidence (tests run, outputs)
- Prefer deterministic, explicit code over cleverness.

---

## 10.3 Agent Roles (Required)

### A) Program Manager Agent (PM)
**Responsibility**
- Maintains roadmap, milestones, and scope.
- Ensures requirements are written and uniquely identified.

**Outputs**
- `docs/requirements/*`, milestone checklist, release notes.

**Acceptance**
- Every feature has an ID and a measurable acceptance criterion.

---

### B) System Architect Agent
**Responsibility**
- Defines module boundaries, IPC, data models, and performance budgets.

**Outputs**
- `docs/architecture/*`, interface schemas, repo scaffolding.

**Acceptance**
- No cyclic deps, clear separation: safety core vs UI vs infotainment plugins.

---

### C) Safety Engineer Agent (ISO 26262)
**Responsibility**
- Produces safety artifacts and constraints.
- Defines safety mechanisms: validity, plausibility, watchdogs, degraded modes.

**Outputs**
- `docs/safety/*` including HARA + safety requirements + verification plan.

**Acceptance**
- Safety requirements traced to tests (at least stub tests in v1).

---

### D) SOTIF Engineer Agent
**Responsibility**
- Identifies “works as intended but unsafe” scenarios (misuse, edge environments).

**Outputs**
- `docs/safety/sotif_scenarios.md`, scenario-driven test cases.

**Acceptance**
- At least N scenarios per critical function (speed display, alerts).

---

### E) Cybersecurity Engineer Agent (ISO/SAE 21434 + R155)
**Responsibility**
- TARA, security requirements, permissions model, secure update hooks.

**Outputs**
- `docs/security/*`, threat model diagrams, security test plan.

**Acceptance**
- Security requirements mapped to code controls + tests (mock acceptable in v1).

---

### F) QML UI Engineer Agent (Design System)
**Responsibility**
- Builds reusable QML components, tokens, animations, responsive layouts.

**Outputs**
- `shared/qml/DesignSystem/*`, view QML files for both apps.

**Acceptance**
- Passes `qmllint`; components documented; performance budgets respected.

---

### G) C++ Platform Engineer Agent
**Responsibility**
- SignalHub, validation, IPC, scheduling, logging, models.

**Outputs**
- `shared/cpp/*`, IPC schemas, platform tests.

**Acceptance**
- Deterministic behavior; static analysis clean; tests pass.

---

### H) Driver UI Safety Core Engineer Agent
**Responsibility**
- Implements driver state model, alert manager, telltale logic, degraded mode.

**Outputs**
- `driver_ui/safety_core/*`, robust tests, fault injection harness.

**Acceptance**
- Meets SR-* requirements; passes fault injection tests.

---

### I) Infotainment App Engineer Agent
**Responsibility**
- App shell, navigation between modules, media mock, settings framework.

**Outputs**
- `infotainment_ui/app/*`, plugin scaffolding.

**Acceptance**
- Cyber requirements CR-* implemented at least as enforceable scaffolds.

---

### J) QA / Verification Agent
**Responsibility**
- Adds tests, CI steps, regression scenarios, traceability generator.

**Outputs**
- CI configs, test suites, `tools/traceability/*`.

**Acceptance**
- CI green; traceability report produced on build.

---

### K) Documentation Agent
**Responsibility**
- Keeps docs synchronized with code; writes developer onboarding.

**Outputs**
- `README.md`, `docs/process/*`, contribution guidelines.

**Acceptance**
- New developer can build + run both apps within a documented sequence.

---

## 10.4 Agent Collaboration Workflow (Mandatory)
1) PM creates/updates requirement(s) with IDs.
2) Architect defines design notes + interfaces (if needed).
3) Safety/Cyber review constraints and add requirements.
4) Implementation agents (QML/C++) produce code + tests.
5) QA agent enforces gates + updates traceability.
6) Doc agent finalizes documentation and examples.

---

## 10.5 Agent Prompt Templates (Concise, enforceable)

### Universal Patch Prompt
- “Implement only the changes needed for requirement IDs: [IDs].  
  Add/modify tests proving acceptance criteria.  
  Update traceability mapping.  
  Do not modify unrelated files.”

### Driver Safety Core Prompt Addendum
- “No safety logic in QML/JS.  
  Validate inputs (range, units, plausibility, freshness).  
  Implement degraded mode.  
  Add fault injection tests for each hazard scenario.”

### Infotainment Security Prompt Addendum
- “Implement permission checks and secure defaults.  
  Avoid logging sensitive data.  
  Add at least one test per security requirement.”

---

## 11) Feature Backlog (v1 → v2)

### v1 (Desktop-ready, standards-ready scaffolding)
- Repo scaffolding + build + CI
- Design system + basic navigation
- Driver UI: speed/gear/telltales/alerts + degraded mode
- Infotainment: home/media/settings + plugin scaffold
- Safety + security docs (HARA/TARA initial)
- Simulation: scenario runner + signal replay

### v2 (Advanced UX + richer evidence)
- Navigation integration placeholder (map provider abstraction)
- More driver modes (range/efficiency/ADAS)
- Security hardening: sandbox runner, signed plugin manifest
- Automated UI regression tests (screenshots, perf)

---

## 12) Deliverables Checklist
- [ ] Both apps build and run on Linux + one of Windows/macOS
- [ ] Shared design system with documented components
- [ ] Driver UI safety core with fault injection tests
- [ ] Infotainment permission-gated settings framework
- [ ] Safety docs: safety plan, HARA, safety requirements, verification plan
- [ ] Security docs: cybersecurity plan, TARA, security requirements, test plan
- [ ] CI enforcing lint + static analysis + tests + traceability

---

## Appendix A — Requirement ID Conventions
- System: `SYS-###`
- Driver Cluster: `SR-CL-###`
- Infotainment: `FR-INF-###` (functional), `CR-INF-###` (cyber)
- Safety process artifacts: `SAFE-###`
- Verification: `VER-###`

---

## Appendix B — Performance Budgets (Targets)
- Driver UI:
  - Render: 60fps target; critical updates ≥10Hz
  - Signal freshness: stale indicator within 300ms missed update
- Infotainment:
  - Core views: 60fps target
  - App transitions: <250ms perceived latency
