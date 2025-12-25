# Automotive UI Suite

A Qt 6 + QML automotive UI framework with two subprojects:

1. **Driver UI (Cluster)** - Safety-relevant gauges, telltales, and alerts
2. **Infotainment UI** - Media, navigation, phone, and vehicle settings

## Compliance Targets

| Standard | Scope | Artifacts |
|----------|-------|-----------|
| ISO 26262 | Functional Safety | `docs/safety/` |
| ISO/PAS 21448 (SOTIF) | Performance Limitations | `docs/safety/sotif_scenarios.md` |
| ISO/SAE 21434 | Cybersecurity | `docs/security/` |
| UNECE R155 | Cybersecurity Management | `docs/security/` |
| ASPICE | Process Maturity | All work products |
| MISRA C++ | Coding Standards | Static analysis config |

## Quick Start

### Prerequisites

- Qt 6.5+ (LTS recommended)
- CMake 3.21+
- C++17 compiler (GCC 11+, Clang 14+, MSVC 2022+)
- Python 3.8+ (for tools)

### Build

```bash
# Linux/macOS
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
cmake --build . --parallel

# Windows (PowerShell)
mkdir build; cd build
cmake .. -DCMAKE_PREFIX_PATH=C:/Qt/6.5.0/msvc2022_64
cmake --build . --config Release
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_DRIVER_UI` | ON | Build Driver UI executable |
| `BUILD_INFOTAINMENT_UI` | ON | Build Infotainment UI executable |
| `ENABLE_SIM` | ON | Enable simulation/mock adapters |
| `ENABLE_TESTS` | ON | Build test suites |
| `ENABLE_STATIC_ANALYSIS` | OFF | Enable clang-tidy during build |

### Run

```bash
# Linux/macOS
./tools/run_desktop.sh driver      # Launch Driver UI
./tools/run_desktop.sh infotainment # Launch Infotainment UI
./tools/run_desktop.sh both        # Launch both (separate processes)

# Windows
.\tools\run_desktop.ps1 driver
.\tools\run_desktop.ps1 infotainment
.\tools\run_desktop.ps1 both
```

### Test

```bash
cd build
ctest --output-on-failure

# Or run specific test suites
./tests/test_safety_core
./tests/test_signal_validation
./tests/test_infotainment_security
```

### Static Analysis & Linting

```bash
# Format check
./tools/format_check.sh

# Run clang-tidy
./tools/run_clang_tidy.sh

# QML lint
./tools/qml_lint.sh
```

### Traceability Report

```bash
python3 tools/traceability/generate_report.py
# Output: reports/traceability_report.html
```

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Desktop / Vehicle Platform                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────┐        ┌─────────────────────────────┐ │
│  │    Driver UI        │   IPC  │     Infotainment UI         │ │
│  │   (Cluster)         │◄──────►│                             │ │
│  │                     │        │  ┌─────┐ ┌─────┐ ┌───────┐  │ │
│  │  ┌──────────────┐   │        │  │Home │ │Media│ │Vehicle│  │ │
│  │  │  QML Views   │   │        │  └─────┘ └─────┘ └───────┘  │ │
│  │  └──────────────┘   │        │  ┌─────┐ ┌─────┐ ┌───────┐  │ │
│  │         │           │        │  │Phone│ │ Nav │ │ Apps  │  │ │
│  │  ┌──────────────┐   │        │  └─────┘ └─────┘ └───────┘  │ │
│  │  │ Safety Core  │   │        │                             │ │
│  │  │   (C++)      │   │        │  ┌─────────────────────────┐│ │
│  │  └──────────────┘   │        │  │    App Framework (C++)  ││ │
│  │         │           │        │  └─────────────────────────┘│ │
│  └─────────┼───────────┘        └─────────────┼───────────────┘ │
│            │                                  │                 │
├────────────┼──────────────────────────────────┼─────────────────┤
│            │        Shared Platform Layer     │                 │
│  ┌─────────┴──────────────────────────────────┴───────────────┐ │
│  │  SignalHub │ IPC │ Logging │ Scheduler │ DesignSystem      │ │
│  └────────────────────────────────────────────────────────────┘ │
│                              │                                  │
├──────────────────────────────┼──────────────────────────────────┤
│                    Adapters  │                                  │
│            ┌─────────────────┼───────────────────┐              │
│            │ Sim │ Mock │ SocketCAN (future)     │              │
│            └─────────────────────────────────────┘              │
└─────────────────────────────────────────────────────────────────┘
```

## Safety Partitioning

- **Driver UI** and **Infotainment UI** run as **separate processes**
- **No safety-critical logic in QML/JS** for Driver UI
- Driver UI uses a **deterministic C++ safety core**:
  - Bounded memory allocations after init
  - Fixed update tick (60Hz render, 20Hz signal)
  - Validated signal ranges and freshness
- IPC is explicit, minimal, versioned, and hardened

## Documentation

### Safety & Security
- [Safety Plan](docs/safety/Safety_Plan.md)
- [HARA (Hazard Analysis and Risk Assessment)](docs/safety/HARA.md)
- [SOTIF Scenarios](docs/safety/SOTIF_Scenarios.md)
- [Functional Safety Concept](docs/safety/Functional_Safety_Concept.md)
- [Technical Safety Concept](docs/safety/Technical_Safety_Concept.md)
- [Safety Case](docs/safety/Safety_Case.md)
- [Verification Plan](docs/safety/Verification_Plan.md)
- [Item Definition](docs/safety/Item_Definition.md)

### Cybersecurity
- [Cybersecurity Plan](docs/security/Cybersecurity_Plan.md)
- [TARA (Threat Analysis and Risk Assessment)](docs/security/TARA.md)
- [Security Requirements](docs/security/Security_Requirements.md)
- [Security Test Plan](docs/security/Security_Test_Plan.md)
- [Asset Inventory](docs/security/Asset_Inventory.md)
- [Incident Response](docs/security/Incident_Response.md)

### Implementation
- [Turn Indicators](docs/TURN_INDICATORS.md)
- [Assumptions](docs/assumptions.md)

## License

Copyright (c) 2025. All rights reserved.
