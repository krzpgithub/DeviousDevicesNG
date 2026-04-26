# Devious Devices NG

[![License: Apache-2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
![Platform](https://img.shields.io/badge/platform-windows%20x64-lightgrey)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-brightgreen)
![CMake](https://img.shields.io/badge/CMake-3.21+-blue)
![vcpkg](https://img.shields.io/badge/vcpkg-managed-orange)



> **Devious Devices NG (DDNG)** — a complete rewrite of the original Devious Devices plugin, built with modern C++23, providing a robust native device framework, 3D model hiding, inventory filtering, bondage state tracking, and a stable DLL & Papyrus API.

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Architecture](#architecture)
- [Installation](#installation)
- [Configuration](#configuration)
- [Building](#building)
- [API](#cross-mod-api)
- [Papyrus Integration](#papyrus-integration)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [License](#license)

---

## Overview

Devious Devices NG is an SKSE (Skyrim Script Extender) plugin that provides a comprehensive backend for the Devious Devices framework for wearable bondage devices in Skyrim. It parses device data from ESM/ESP files, manages 3D model visibility, filters inventory and equipment based on device state, tracks bondage conditions, and exposes functions to both Papyrus scripts and other mods via a stable DLL API.

---

## Key Features

| Feature | Description |
|---|---|
| **Device Framework** | Parses wearable device data from ESM/ESP files with full mod override history tracking |
| **3D Model Hiding** | Automatically hides device meshes and body nodes (arms, hands, fingers) when devices are worn |
| **Inventory Filtering** | Prevents picking up or equipping items based on worn devices and configurable keyword rules |
| **Bondage State Tracking** | Bitmask-based state system: hands bound, gagged, chastified, blindfolded, mittens, boots, and more |
| **Facial Expression Management** | Applies gag-related facial expressions for actors with priority-based resolution |
| **Movement Restriction** | Reduces speed, forces walk mode, and disables sprint/combat when bound |
| **Papyrus Integration** | Exposes native functions for mod authors via Papyrus script bindings |
| **DLL API** | Stable DLL API for other mods via `LoadLibrary`/`GetProcAddress` |
| **Save/Load Serialization** | Persists device state, manipulation flags, and actor data across game loads |
| **Configurable INI** | Extensive runtime configuration via `DeviousDevices.ini` |

---

## Architecture

### Core Components

| Component | Header | Source | Description |
|---|---|---|---|
| **DeviceReader** | `DeviceReader.h` | `DeviceReader.cpp` | Parses ESM/ESP device data, builds the device database, resolves mod override history, and provides property queries |
| **DeviceHider** | `Hider.h` | `Hider.cpp` | Manages armor rendering for actors, hides device meshes via slot filtering and keyword rules |
| **NodeHider** | `NodeHider.h` | `NodeHider.cpp` | Hides specific NiNode nodes (arms, hands, fingers, weapons) on actor 3D models |
| **Hooks** | `Hooks.h` | `Hooks.cpp` | SKSE detour functions for equip, pickup, movement, and sprint hooks |
| **HooksVirtual** | `HooksVirtual.h` | `HooksVirtual.cpp` | Virtual function hooks for player controls (button processing, thumbstick input) |
| **InventoryFilter** | `InventoryFilter.h` | `InventoryFilter.cpp` | Keyword-based item filtering for pickup and equip operations |
| **MovementManager** | `MovementManager.h` | `MovementManager.cpp` | Manages speed reduction, forced walk mode, and combat restrictions when bound |
| **ExpressionManager** | `Expression.h` | `Expression.cpp` | Applies and manages gag facial expressions with priority resolution and faction-based presets |
| **UpdateManager** | `UpdateManager.h` | `UpdateManager.cpp` | Timed actor state updates (player and NPC) |
| **LibFunctions** | `LibFunctions.h` | `LibFunctions.cpp` | Utility library: bondage state queries, worn device lookup, keyword checks |
| **Papyrus** | `Papyrus.h` | `Papyrus.cpp` | Papyrus script bindings for all exposed functions |
| **Serialization** | `Serialization.h` | `Serialization.cpp` | Save/load state persistence across game loads |
| **ConfigManager** | `Config.h` | `Config.cpp` | INI configuration parsing via Boost property-tree |

### Component Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        Entry Point                          │
│                      Main.cpp / DLLMain                     │
└──────────────────────────┬──────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
┌───────────────┐  ┌───────────────┐  ┌───────────────┐
│ DeviceReader  │  │  ConfigMgr    │  │ Serialization │
│               │  │               │  │               │
│ ESM/ESP parser│  │ INI config    │  │ Save/Load     │
│ DB builder    │  │ Boost PT      │  │               │
└───────┬───────┘  └───────────────┘  └───────────────┘
        │
        ▼
┌───────────────────────────────────────────────────────┐
│                    Runtime Systems                    │
│                                                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐ │
│  │  Hider   │  │ NodeHi-  │  │  Express.│  │Move-   │ │
│  │          │  │ der      │  │  Manager │  │ ment   │ │
│  │ Armor    │  │ Node     │  │  Facial  │  │ Mana-  │ │
│  │ filtering│  │ hiding   │  │expressions│ │ ger    │ │
│  └──────────┘  └──────────┘  └──────────┘  └────────┘ │
│                                                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐ │
│  │Inventory │  │  Update  │  │   Lib    │  │ Papyrus│ │
│  │  Filter  │  │  Manager │  │ Functions│  │  Bind. │ │
│  │ Pickup/  │  │  Timed   │  │  Utils   │  │        │ │
│  │ Equip    │  │  Updates │  │          │  │        │ │
│  └──────────┘  └──────────┘  └──────────┘  └────────┘ │
│                                                       │
│  ┌──────────────────────────────────────────────────┐ │
│  │              Hooks / HooksVirtual                │ │
│  │  SKSE detours: equip, pickup, movement, sprint   │ │
│  └──────────────────────────────────────────────────┘ │
└──────────────────────────┬────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
┌───────────────┐  ┌───────────────┐  ┌───────────────┐
│     DLL       │  │   UI / Menu   │  │  Settings     │
│     API       │  │               │  │               │
│   (DLL v2)    │  │ MessageBox    │  │ Papyrus quest │
│               │  │  system       │  │ settings      │
└───────────────┘  └───────────────┘  └───────────────┘
```

---

## Installation

1. Download the latest release from the GitHub releases page.
2. Extract the contents into your Skyrim Special Edition root directory.
3. Ensure SKSE is installed and working.

**Distribution layout:**

```
<Skyrim Root>/
├── SKSE/
│   ├── Plugins/
│   │   └── DeviousDevices.dll
│   ├── DeviousDevices.ini
│   ├── scripts/              (compiled Papyrus .pex files)
│   └── PartialAnimationReplacer/
│       └── Replacers/
│           └── DeviousDevices/
│               └── *.json    (animation replacer configs)
├── Devious Devices - Assets.esm
├── Devious Devices - Contraptions.esm
├── Devious Devices - Expansion.esm
├── Devious Devices - Integration.esm
└── Devious Devices SE patch.esp
```

---

## Configuration

The plugin is configured via `SKSE/Plugins/DeviousDevices.ini`. Key configuration sections:

### `[Main]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `bPrintDB` | bool | `false` | Print device database to log (debug only) |
| `iLogging` | int | `1` | Log verbosity: 0=Errors, 1=Errors+Warnings, 2=All |

### `[InventoryFilter]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `bGagFilterModeMenu` | bool | `true` | Gag filter only active in inventory menu |
| `bEquipFilterModeMenu` | bool | `true` | Equip filter only active in inventory menu |
| `asWhitelist` | string array | `3BBB Body, SMP` | Keywords to whitelist from filtering |
| `asWhitelistFood` | string array | `Skooma, Lotion` | Food keywords to whitelist |
| `bEquipSpell` | bool | `true` | Allow spell equip when gagged |
| `bEquipShout` | bool | `true` | Allow shout equip when gagged |

### `[DeviceHider]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `bEnabled` | bool | `true` | Enable the device hider system |
| `bNPCsEnabled` | bool | `true` | Hide devices on NPCs |
| `bOnlyDevices` | bool | `false` | Only hide devices worn by other devices |
| `aiHiderOverrideSlots` | int | `60` | Body slots to always keep visible |

### `[NodeHider]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `bEnabled` | bool | `true` | Enable node-based hiding |
| `bHideArms` | bool | `true` | Hide arm nodes |
| `bHideArmsFirstPerson` | bool | `true` | Hide arm nodes in first person |
| `asArmNodes` | string array | (node list) | Arm nodes to hide |
| `asHandNodes` | string array | (node list) | Hand nodes to hide |
| `asFingerNodes` | string array | (node list) | Finger nodes to hide |
| `asWeaponNodes` | string array | (node list) | Weapon nodes to hide |
| `asArmHiddingKeywords` | string array | (keywords) | Keywords that trigger arm hiding |
| `asHandHiddingKeywords` | string array | (keywords) | Keywords that trigger hand hiding |
| `asFingerHiddingKeywords` | string array | (keywords) | Keywords that trigger finger hiding |

### `[Movement]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `asForceWalkKeywords` | string array | `zadNG_EffectWalk` | Keywords to force walk mode |
| `asDisableCombatKeywords` | string array | `zad_BoundCombatDisableKick` | Keywords to disable combat |
| `afMaxSpeedMult` | float | `0.15` | Speed multiplier in force walk mode |
| `afMaxSprintSpeedMult` | float | `0.4` | Sprint speed multiplier in force walk mode |

### `[GagExpression]`
| Setting | Type | Default | Description |
|---|---|---|---|
| `bNPCsEnabled` | bool | `true` | Apply gag expressions on NPCs |
| `iUpdatePlayerTime` | int | `500` | Player expression update interval (ms) |
| `iNPCUpdateTime` | int | `120` | NPC expression update interval (frames at 60 FPS) |
| `iCleanTime` | int | `10` | Actor cleanup interval (seconds) |

---

## Building

### Prerequisites

| Requirement | Minimum Version | Notes |
|---|---|---|
| **CMake** | 3.21 | Build system |
| **Compiler** | MSVC (Visual Studio 2022) or clang-cl | C++23 support required |
| **Platform** | Windows x64 | |
| **vcpkg** | Latest | Dependency management |
| **vcpkg triplet** | `x64-windows-skse` | Custom overlay triplet provided |

### Dependencies

| Package | Version | Purpose |
|---|---|---|
| `commonlibsse-ng` | via vcpkg registry | SKSE framework |
| `detours` | via vcpkg | Function hooking (Microsoft Detours) |
| `boost-algorithm` | 1.86.0 | String algorithms |
| `boost-math` | 1.86.0 | Math utilities |
| `boost-lexical-cast` | 1.86.0 | Type conversion |
| `boost-property-tree` | 1.86.0 | INI config parsing |
| `spdlog` | via CommonLibSSE | Logging |

### Recommended: CMake Presets

The project ships with CMake presets for streamlined builds:

**Configure and build a release:**
```powershell
cmake --preset=build-release-msvc
cmake --build --preset=release-msvc
```

**Configure and build debug:**
```powershell
cmake --preset=build-debug-msvc
cmake --build --preset=debug-msvc
```

**Available presets:**

| Preset Name | Description |
|---|---|
| `build-release-msvc` | Optimized release build (RelWithDebInfo) with MSVC |
| `build-debug-msvc` | Debug build with MSVC |
| `build-release-clang-cl` | Optimized release build with clang-cl |
| `build-debug-clang-cl` | Debug build with clang-cl |
| `release-msvc` | Build preset for release (MSVC) |
| `debug-msvc` | Build preset for debug (MSVC) |
| `tests-all` | Run all tests |
| `tests-unit` | Run unit tests only |

### Manual Build

```powershell
$env:VCPKG_ROOT = "C:\vcpkg"

cmake -S . -B build -G Ninja `
    -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET="x64-windows-skse" `
    -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" `
    -DVCPKG_OVERLAY_TRIPLETS="${PWD}/cmake" `
    -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build build --config RelWithDebInfo
```

**Key CMake variables:**

| Variable | Description |
|---|---|
| `VCPKG_TARGET_TRIPLET` | vcpkg target triplet (`x64-windows-skse`) |
| `VCPKG_OVERLAY_TRIPLETS` | Path to custom overlay triplets directory |
| `CMAKE_MSVC_RUNTIME_LIBRARY` | Runtime linkage (`MultiThreadedDLL` / `MultiThreadedDebugDLL`) |
| `BUILD_TESTS` | Enable unit tests (`ON`/`OFF`) |

### Deployment

Set the `PluginDeployTarget` environment variable for automatic deployment:

```powershell
$env:PluginDeployTarget = "C:\Program Files\Steam\steamapps\common\Skyrim Special Edition"
cmake --build build --config RelWithDebInfo
```

This copies `DeviousDevices.dll` (and `.pdb` for Debug/RelWithDebInfo) to `<SkyrimDir>/SKSE/Plugins/`.

---

## DLL API

DDNG provides a stable DLL API that other mods can consume via `LoadLibrary`/`GetProcAddress`.

### Loading the API

```cpp
#include "API.h"

SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
        {
            if (DeviousDevicesAPI::LoadAPI()) {
                auto* api = DeviousDevicesAPI::g_API;
                auto devices = api->GetWornDevices(actor);
            }
        }
        break;
    }
});
```

### API Overview

| Category | Functions |
|---|---|
| **Device Reader** | `GetDatabase()`, `GetDeviceRender()`, `GetDeviceInventory()`, `GetPropertyForm()`, `GetPropertyInt()`, `GetPropertyFloat()`, `GetPropertyBool()`, `GetPropertyString()` |
| **Expressions** | `ApplyExpression()`, `ResetExpression()`, `UpdateGagExpression()`, `ResetGagExpression()`, `IsGagged()`, `RegisterGagType()` |
| **Hider** | `SetActorStripped()`, `IsActorStripped()`, `IsValidForHide()` |
| **Lib Functions** | `GetDevices()`, `GetWornDevice()`, `GetWornDevices()`, `GetHandRestrain()`, `GetBondageState()`, `IsDevice()`, `ActorHasBlockingGag()` |

### BondageState Bitmask

```cpp
enum BondageState : uint32_t {
    sNone               = 0x0000,
    sHandsBound         = 0x0001,
    sHandsBoundNoAnim   = 0x0002,
    sGaggedBlocking     = 0x0004,
    sChastifiedGenital  = 0x0008,
    sChastifiedAnal     = 0x0010,
    sChastifiedBreasts  = 0x0020,
    sBlindfolded        = 0x0040,
    sMittens            = 0x0080,
    sBoots              = 0x0100,
    sTotal              = 0x0200
};
```

---

## Papyrus Integration

DDNG exposes a rich set of functions to Papyrus scripts, allowing mod authors to:

- Query worn devices on any actor
- Get bondage state information
- Retrieve device properties 
- Apply/reset facial expressions
- Check device rendering mappings

---

## Testing

Unit tests are available but disabled by default. Enable them during configuration:

```powershell
# Using presets
cmake --preset=build-debug-msvc
cmake --build --preset=debug-msvc

# Run tests
cmake --preset=tests-all
```

---

## Project Structure

```
DeviousDevicesNG/
├── CMakeLists.txt              # Root CMake build configuration
├── CMakePresets.json           # CMake presets (configure + build + test)
├── vcpkg.json                  # vcpkg manifest (dependencies)
├── vcpkg-configuration.json    # vcpkg registries
├── LICENSE                     # Apache License 2.0
├── README.md                   # This file
├── .clang-format               # Code style configuration
│
├── include/                    # Public headers 
│   ├── API.h                   # DLL API 
│   ├── Export.h                # Exported symbols (legacy)
│   ├── DeviceReader.h          # Device parsing & database
│   ├── Hider.h                 # Armor/device hiding
│   ├── NodeHider.h             # 3D node hiding
│   ├── Expression.h            # Gag facial expressions
│   ├── InventoryFilter.h       # Item filtering
│   ├── MovementManager.h       # Movement restriction
│   ├── UpdateManager.h         # Timed actor updates
│   ├── LibFunctions.h          # Utility functions
│   ├── Papyrus.h               # Papyrus bindings
│   ├── Serialization.h         # Save/load persistence
│   ├── Config.h                # INI config parsing
│   ├── Hooks.h                 # SKSE detour hooks
│   ├── HooksVirtual.h          # Virtual function hooks
│   ├── UI.h                    # UI/MessageBox utilities
│   ├── Utils.h                 # Shared utilities
│   ├── Script.hpp              # Script utility helpers
│   ├── Switches.h              # Preprocessor feature toggles
│   └── Settings.h              # Papyrus quest settings access
│
├── src/                        # Source files 
│   ├── Main.cpp                # DLL entry point, initialization
│   ├── DeviceReader.cpp        # Device parsing implementation
│   ├── Hider.cpp               # Device hider implementation
│   ├── NodeHider.cpp           # Node hiding implementation
│   ├── Expression.cpp          # Expression management
│   ├── InventoryFilter.cpp     # Inventory filtering
│   ├── MovementManager.cpp     # Movement restriction
│   ├── UpdateManager.cpp       # Timed updates
│   ├── LibFunctions.cpp        # Utility functions
│   ├── Papyrus.cpp             # Papyrus function registration
│   ├── Serialization.cpp       # Save/load implementation
│   ├── Config.cpp              # INI config parsing
│   ├── HooksVirtual.cpp        # Virtual hook implementation
│   ├── API.cpp                 # DLL API implementation
│   ├── Export.cpp              # Legacy export functions
│   └── Utils.cpp               # Utility implementations
│
├── cmake/                      # CMake build helpers
│   ├── x64-windows-skse.cmake  # Custom vcpkg triplet
│   └── version.rc.in           # Windows version resource template
│
├── test/                       # Unit test source
│   └── DeviousDevices.cpp      # Catch2 test suite
│
├── dist/                       # Distribution layout
│   ├── SKSE/
│   │   ├── Plugins/
│   │   └── PartialAnimationReplacer/
│   ├── scripts/
│   └── Seq/                    # Sequence files
│
├── *.esm / *.esp               # Data files (device definitions)
│
└── .github/
    └── workflows/
        └── build.yml           # CI/CD pipeline
```

---

## CI/CD

The project uses GitHub Actions with the following pipeline:

1. **`check-changes`** — Checks for recent changes (daily + fortnightly schedule)
2. **`build-skse`** — Builds the SKSE plugin DLL (Windows, MSVC)
3. **`build-papyrus`** — Compiles Papyrus source files
4. **`compose-release`** — Assembles the final release artifact
5. **`release-nightly`** — Creates nightly releases on schedule

Nightly releases are published as GitHub releases with the tag format `nightly-YYYYMMDD`.

---

## License

This project is licensed under the **Apache License 2.0**.

```
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

---

*Devious Devices NG is a community project and is not affiliated with the original Devious Devices mod.*
