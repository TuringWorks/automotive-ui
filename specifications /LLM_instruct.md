Instructions to Coding Agents

You are an expert automotive software architect and senior Qt/QML engineer. You must implement an automotive UI suite with TWO subprojects using Qt 6 + QML:
1) Driver UI (Cluster) with gauges/controls (safety-relevant)
2) Infotainment UI (non-safety but cybersecurity-critical)

You MUST follow the specifications exactly as the source of truth in these files:
- Automotive_UI_LLM_Agents_Spec.md
- Safety_HARA_SOTIF_Skeleton.md
- Cybersecurity_TARA_Skeleton.md
- Traceability_Template.md

RULE 0 — DO NOT ASK ME QUESTIONS unless you are blocked by missing required data. If uncertain, make a reasonable assumption and document it in /docs/assumptions.md.

RULE 1 — Safety partitioning is mandatory:
- Driver UI and Infotainment UI must be separate processes.
- NO safety-critical logic in QML/JS for Driver UI. QML is rendering/bindings only.
- Driver UI must have a deterministic C++ “safety core” (bounded allocations after init, fixed update tick, validated signal ranges).
- IPC between apps must be explicit, minimal, versioned, and hardened.

RULE 2 — Compliance-ready structure is mandatory:
- Produce docs scaffolding that is ISO 26262 + SOTIF + ASPICE-ready and ISO/SAE 21434 + UNECE R155-ready using the provided skeleton files.
- Maintain requirement IDs and traceability mapping.

RULE 3 — Quality gates are mandatory:
- Include CMake build for Qt6 on Linux/Windows/macOS.
- Include lint/format and static analysis scaffolding: clang-format, clang-tidy (MISRA-inspired), qmllint, qmlformat.
- Include unit tests (C++) and QML tests plus basic UI smoke-test scripts.
- Generate/update a traceability report (Req → Design → Code → Tests → Evidence).

OUTPUT FORMAT REQUIREMENTS:
- You will produce a complete repository layout exactly matching the spec.
- For each file you create or modify, output it in a structured “FILE:” format:
  FILE: path/to/file
  <entire file contents>
- Do not omit files. If a file is a placeholder, include it with TODO markers.
- At the end, output:
  1) Build instructions for Linux, Windows, macOS
  2) How to run Driver UI and Infotainment UI on desktop
  3) How to run tests and produce traceability output
  4) A short “Compliance Notes” section mapping key deliverables to standards

IMPLEMENTATION TASKS (do in order):
1) Create repo skeleton and CMake build that can build two executables: driver_ui and infotainment_ui.
2) Implement shared/ platform layer:
   - signal model + validation
   - deterministic scheduler / tick
   - logging + metrics
   - IPC schema(s) and minimal IPC stub
   - QML design system (tokens/components)
3) Implement driver_ui:
   - C++ safety core with signal freshness, range checks, degraded mode
   - QML cluster UI (speed, gear, telltales, alerts overlay)
   - Fault injection simulator scenarios + tests for SR-CL-001..004 at minimum
4) Implement infotainment_ui:
   - QML shell (home/media/settings/app drawer) + minimal mock apps
   - Permission-gated “vehicle settings” actions (scaffold)
   - Security tests for CR-INF-* scaffolds
5) Add docs:
   - Populate ISO 26262 safety docs using the skeleton
   - Populate SOTIF scenarios section with initial scenarios and mitigations
   - Populate ISO/SAE 21434 security docs using the skeleton
6) Add traceability:
   - Create tools/traceability scrip

   