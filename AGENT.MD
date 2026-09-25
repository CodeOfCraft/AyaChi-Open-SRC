# Ayachi Agent Instructions

This file is the primary guidance for AI coding agents working in this repository. Read it before changing code, together with `README.md` and any more-specific instructions in the directory being modified.

## Repository Overview

Ayachi is a Windows Minecraft Bedrock Edition client DLL written in C++20. The main source tree is under `Ayachi/`:

- `Client/`: client lifecycle, managers, commands, and feature modules.
- `Renderer/`: DirectX/ImGui rendering and overlay helpers.
- `SDK/`: Minecraft types, runtime interfaces, and network/world models.
- `Utils/`: shared memory, string, color, logging, and utility code.
- `Libs/`: bundled third-party libraries.
- `dllmain.cpp`: DLL entry point.

This project uses runtime injection and function hooks. SDK types, addresses, calling conventions, and render/input hooks are sensitive to game-version and ABI changes. Keep changes localized and document relevant assumptions.

## Environment and Build

Required environment: Windows, Visual Studio 2022 with the Desktop development with C++ workload, the Windows 10 SDK, and MSBuild. The project builds a dynamic library for x64 using C++20 and a precompiled header (`pch.h`).

Run a Release build from the repository root:

```powershell
msbuild Ayachi/Ayachi.vcxproj /m /p:Configuration=Release /p:Platform=x64
```

Alternatively, open `Ayachi/Ayachi.vcxproj` in Visual Studio and build `Release | x64`. Typical outputs are `Ayachi/x64/Release/Ayachi.dll` and its PDB; do not commit generated outputs. For a Debug build:

```powershell
msbuild Ayachi/Ayachi.vcxproj /m /p:Configuration=Debug /p:Platform=x64
```

CI configuration is in `.github/workflows/build.yml`. It builds x64 Release and uploads the DLL/PDB artifacts. There is no standalone unit or integration test suite. If the target game cannot be run, explicitly report that runtime validation was not performed.

The project file currently declares toolset `v145`, while the CI command explicitly passes `PlatformToolset=v143`. Use the installed toolset that matches the project when building locally, and check CI whenever build configuration changes.

## Instructions for Code Changes

- Follow existing directory layout, naming, include style, and manager/module patterns. Do not introduce a new framework or build system for a small change.
- Put public interfaces in headers and implementations in the matching `.cpp` files. When adding a source file, update both `Ayachi.vcxproj` and `Ayachi.vcxproj.filters`; otherwise Visual Studio or CI may not compile it.
- Preserve UTF-8 source handling and the existing `/utf-8` compiler option. Avoid unrelated formatting or broad rewrites.
- Prefer RAII, explicit ownership, and existing utility classes. Keep raw pointers, offsets, casts, and hook-specific code at SDK or hook boundaries where they are required.
- Do not commit generated directories or files such as `x64/`, `Debug/`, `Release/`, `.pdb`, `.obj`, or `.pch`, nor local game data or machine-specific configuration.
- For network, memory, rendering, or input changes, inspect thread safety, object lifetime, and unload behavior. Avoid blocking work or exceptions inside hook callbacks.
- Preserve existing behavior outside the requested change. Do not silently change addresses, packet layouts, public APIs, or configuration formats.

## Validation Checklist

Before completing a change:

1. Review `git diff` and confirm that only task-related files changed.
2. Build `Release | x64`; build Debug as well when the change affects configuration, assertions, or lifecycle behavior.
3. For module, command, or configuration changes, verify registration, serialization, toggling, and unload paths.
4. For SDK or hook changes, record the target Minecraft version, address/ABI assumptions, and whether in-game validation was completed.
5. Report the build command used and any unperformed runtime or test validation.

## Commit Messages

Use the repository's Conventional Commits style, for example `feat: ...`, `fix: ...`, `refactor: ...`, or `docs: ...`. Keep the subject short, imperative, and specific about the behavior or boundary that changed.
