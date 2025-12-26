You are a senior automotive software architect and Qt/QML + Qt Quick 3D engineer. Your task is to generate a complete multi-platform Qt 6 repository that implements TWO apps (separate processes) plus ADAS/Autonomy UI capabilities (L1–L5), and includes safety/cyber/traceability artifacts and tests.

SOURCE OF TRUTH (MUST FOLLOW EXACTLY)
You MUST treat the following Markdown files as the authoritative specification. Do not contradict them:
1) Automotive_UI_LLM_Agents_Spec.md
2) Safety_HARA_SOTIF_Skeleton.md
3) Cybersecurity_TARA_Skeleton.md
4) Traceability_Template.md
5) ADAS_Autonomy_UI_Spec_v2_EosAligned.md
6) ADAS_Safety_Requirements_Addendum_v2_EosAligned.md
7) ADAS_Cybersecurity_Addendum_v2_EosAligned.md

CRITICAL CONSTRAINTS (HARD RULES)
- Driver UI (cluster) and Infotainment UI must be separate executables/processes.
- NO safety-critical logic in QML/JS for Driver UI. QML is rendering/bindings only. All safety/ADAS gating, timing, validity, state machines, takeover logic are deterministic C++.
- Infotainment may be richer and modular, but must not be the sole source of any safety-critical ADAS state; Driver UI is authoritative.
- IPC must be schema-based, versioned, validated, and hardened. Treat IPC as untrusted input.
- Implement Eos-aligned features in the ADAS spec: environment presets (dawn/day/dusk/night), weather particles, reflection probes, skeletal animation support, and Event List scenario replay tooling (engineering-only + motion locked).
- Implement the “Benchmark-inspired” quality manager controlling 3D fidelity knobs (particles, reflections, LOD, MSAA, dynamic resolution) without ever removing safety-critical cues.

OUTPUT FORMAT (MANDATORY)
Produce the repository as a set of files using the exact format below for EVERY file you create or modify:

FILE: path/to/file
<entire file contents>

Do not skip files. If something is a placeholder, include it with TODO markers and clear next steps.

REPOSITORY REQUIREMENTS
- Use Qt 6 + QML; include Qt Quick 3D for ADAS 3D visualization surfaces.
- Use CMake as the build system.
- Provide two executables:
  - driver_ui (cluster)
  - infotainment_ui
- Provide shared libraries/modules under shared/ for:
  - signal model + validation (timestamp/validity/confidence/seq/units)
  - deterministic scheduler/tick
  - IPC schemas and message codecs
  - logging + metrics
  - ADAS state service models (AdasStateService, OddService, PerceptionModel, TakeoverManager, DriverMonitoringService, HmiEventLog)
  - AdasVisualQualityManager (benchmark-like tuning knobs)
- Provide tools/ for:
  - scenario/event replay (EventListModel + EventListSimulator)
  - scenario runner CLI for replaying ADAS scenes
  - traceability generator (Req → Design → Code → Test → Evidence) producing reports under /reports

FEATURE REQUIREMENTS (IMPLEMENT MINIMUM VIABLE SET)
A) Driver UI
- Basic cluster: speed, gear, telltales, alerts overlay, degraded mode.
- ADAS mini-view: lane corridor + lead vehicle + adjacent object markers (2D + optional 3D).
- Full takeover/MRM banner that cannot be occluded.
- Implement Driver UI state machine per ADAS_Autonomy_UI_Spec_v2_EosAligned.md.
- Implement at least these safety requirements with tests and evidence:
  - SR-CL-001..004 (from base spec)
  - SR-CL-ADAS-100/101/110/111/120/130/140/150 (and Eos-aligned: SR-CL-ADAS-170..174)
  - SOTIF-UI-010..011

B) Infotainment UI
- App shell: Home, Media mock, Vehicle settings mock, Autonomy Center (new), App Drawer.
- Autonomy Center: capability overview, ODD viewer, explanations (“why unavailable”), event timeline.
- Engineering-only “Perception Explorer” with:
  - EventListSimulator controls (play/pause/seek)
  - environment presets toggles (dawn/day/dusk/night)
  - weather particle toggles
  - quality manager debug overlay
  - MUST be authorization-gated and motion-locked.

C) Eos-aligned 3D/visual layer
- Implement environment presets (dawn/day/dusk/night) non-blocking.
- Add weather particles (rain/snow/mist).
- Add reflection probes (quality scaled).
- Add skeletal animation support hooks (optional usage).
- Maintain Design Studio friendly structure:
  - asset_imports/Quick3DAssets/...
  - designer/plugin.metainfo for custom components used in tools views.

QUALITY GATES + TESTING (MANDATORY)
- Provide:
  - clang-format config
  - clang-tidy config (MISRA-inspired)
  - qmllint + qmlformat usage scripts
- Tests:
  - C++ unit tests for signal validation, ADAS state machine, takeover timing, staleness
  - QML tests for key UI components
  - Scenario replay tests (fault injection)
  - Security tests for IPC validation, replay rejection, authorization gating, and “no debug controls in production build”
- Provide scripts:
  - build + run driver_ui/infotainment_ui on Linux/macOS/Windows
  - run tests and generate reports
  - generate traceability output

DOCS + TRACEABILITY (MANDATORY)
- Populate docs from the provided skeletons (at least baseline content filled, not empty).
- Add docs/adas/* deliverables listed in ADAS spec.
- Update/create a traceability table linking:
  - Requirements → Design docs → Implementation files → Tests → Evidence artifacts.
- Place evidence outputs under:
  - reports/test/*.json
  - reports/static/*.txt
  - reports/metrics/*.csv

WORK METHOD (DO NOT ASK QUESTIONS)
- Make reasonable assumptions and record them in docs/assumptions.md.
- Use clear, maintainable code and comments.
- Avoid unnecessary complexity; focus on correctness and compliance structure.
- Ensure everything builds without proprietary tooling.

DELIVERABLES AT END OF OUTPUT
After all FILE blocks, print:
1) Build & run instructions for Linux, macOS, Windows
2) Test commands and how to view reports
3) Traceability generation command
4) A short “Compliance Notes” mapping key artifacts to ISO 26262, SOTIF, ASPICE, ISO/SAE 21434, UNECE R155

Now generate the complete repository in FILE: format. 
