# Damascene

[![Discord](https://img.shields.io/badge/Discord-Join%20Server-7289DA?logo=discord&logoColor=white)](https://discord.gg/nfEHFA5n4m)

![Project Logo](_logo.png)

**Damascene** is a modern, cross-platform voxel game engine written in C++. It is designed with modularity, performance, and maintainability in mind, providing a solid foundation for creating voxel-based games and interactive worlds.

The project emphasizes clean architecture, modern C++ practices, and comprehensive documentation to make development easier for both contributors and users.

---

## Features

### Modern C++ Architecture

Damascene is built around a modular engine architecture that prioritizes readability, maintainability, and long-term scalability. Components are designed to be independent wherever possible, making the engine easier to extend and evolve.

### Cross-Platform

The engine is designed to support multiple desktop platforms from the beginning:

* Windows
* Linux
* macOS

Platform-specific code is isolated to keep the core engine portable.

### Performance-Oriented

Damascene targets 64-bit platforms and leverages modern C++ features to improve performance, memory management, and reliability. The engine is being developed with scalability in mind, from small sandbox projects to larger multiplayer experiences.

### Rendering

Damascene uses **bgfx** as its rendering abstraction layer, enabling support for multiple graphics APIs through a unified interface. This allows the engine to run across different platforms while taking advantage of the most appropriate graphics backend available.

Depending on the platform and configuration, bgfx can utilize rendering backends such as **Direct3D**, **OpenGL**, **OpenGL ES**, **Metal**, **Vulkan**, and **WebGPU** (where supported). This approach provides flexibility, simplifies cross-platform development, and allows the renderer to evolve without requiring significant changes to higher-level engine systems.

### Scripting

Damascene is designed to support **AngelScript** as its primary scripting language, allowing developers to implement gameplay logic, create custom behaviors, and rapidly prototype features without recompiling the engine.

The scripting system will expose a stable and well-documented API, enabling game code to interact with engine systems such as entities, components, input, physics, audio, and rendering. By separating gameplay logic from the engine core, AngelScript provides a flexible workflow while keeping the underlying engine performant and maintainable.

### Networking

Networking is being designed as a first-class engine feature.

Planned functionality includes:

* Dedicated server support
* Extensible networking architecture
* Custom multiplayer protocols
* Server-side plugin support

These systems are intended to support everything from small private worlds to large community-hosted servers.

---

## Goals

Damascene aims to provide:

* Clean, well-documented source code
* Modern C++ development practices
* Cross-platform compatibility
* High performance and scalability
* Extensible engine systems
* Long-term maintainability
* A strong foundation for voxel-based games

---

## Project Status

Damascene is currently under active development. APIs, engine systems, and project structure may change as development progresses.

Contributions, bug reports, feature suggestions, and design discussions are welcome as the engine continues to evolve.


## Code Style

[Read the code style guide](CODE_STYLE.md)

---
## Change/Updates Log
[Check out the updates to see the planned features](UPDATES.md)
