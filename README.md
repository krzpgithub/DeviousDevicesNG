# Devious Devices NG

[![License: Apache-2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
![Platform](https://img.shields.io/badge/platform-windows%20x64-lightgrey)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-brightgreen)
![CMake](https://img.shields.io/badge/CMake-3.21+-blue)
![vcpkg](https://img.shields.io/badge/vcpkg-managed-orange)



> **Devious Devices NG (DDNG)** — a complete rewrite of the original Devious Devices plugin, built with modern C++23, providing a robust native device framework, 3D model hiding, inventory filtering, bondage state tracking, and a stable DLL & Papyrus API.

---

## Architecture

### Core Components

| Component | Description |
|---|---|
| **DeviceReader.cpp** | Parses ESM/ESP device data, builds the device database, resolves mod override history, and provides property queries |
| **DeviceHider.cpp** | Manages armor rendering for actors, hides device meshes via slot filtering and keyword rules |
| **NodeHider.cpp** | Hides specific NiNode nodes (arms, hands, fingers, weapons) on actor 3D models |
| **Hooks.cpp** | SKSE detour functions for equip, pickup, movement, and sprint hooks |
| **HooksVirtual.cpp** | Virtual function hooks for player controls (button processing, thumbstick input) |
| **InventoryFilter.cpp** | Keyword-based item filtering for pickup and equip operations |
| **MovementManager.cpp** | Manages speed reduction, forced walk mode, and combat restrictions when bound |
| **ExpressionManager.cpp** | Applies and manages gag facial expressions with priority resolution and faction-based presets |
| **UpdateManager.cpp** | Timed actor state updates (player and NPC) |
| **LibFunctions.cpp** | Utility library: bondage state queries, worn device lookup, keyword checks |
| **Papyrus.cpp** | Papyrus script bindings for all exposed functions |
| **Serialization.cpp** | Save/load state persistence across game loads |
| **ConfigManager.cpp** | INI configuration parsing via Boost property-tree |

---

## DLL API Overview

| Category | Functions |
|---|---|
| **Device Reader** | `GetDatabase()`, `GetDeviceRender()`, `GetDeviceInventory()`, `GetPropertyForm()`, `GetPropertyInt()`, `GetPropertyFloat()`, `GetPropertyBool()`, `GetPropertyString()` |
| **Expressions** | `ApplyExpression()`, `ResetExpression()`, `UpdateGagExpression()`, `ResetGagExpression()`, `IsGagged()`, `RegisterGagType()` |
| **Hider** | `SetActorStripped()`, `IsActorStripped()`, `IsValidForHide()` |
| **Lib Functions** | `GetDevices()`, `GetWornDevice()`, `GetWornDevices()`, `GetHandRestrain()`, `GetBondageState()`, `IsDevice()`, `ActorHasBlockingGag()` |

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

*Devious Devices NG is a community project and is not affiliated with the original Devious Devices mod.*
