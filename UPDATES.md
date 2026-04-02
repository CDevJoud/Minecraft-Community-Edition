# Updates & Development Roadmap

This document tracks all released updates and features currently in development.

---

# 📦 Released Versions

## Version 0.1.0 – Core Architecture Foundation

**Status:** Completed    
**Type:** Initial Engine Foundation  

![Project Logo](snapshotv0.1.0.webp)

Version 0.1.0 establishes the structural and architectural backbone of the engine.  
This release focuses entirely on clean system design and long-term scalability.

### 🧱 Core Architecture
- OOP-structured main entry point
- Modular engine layout
- Application layer abstraction
- Clear separation between core and higher-level systems

### 🖥 Systems Implemented
- Logging system (console + file output)
- File I/O system
- Queued event bus system
- Function container system
- Coroutine scheduler

### 🧠 Design Goals Achieved
- Clean system boundaries
- Modern C++ architecture
- Reduced global state
- Extensible and modular structure
- Cross-platform-ready foundation

---
# 🚧 In Development

## Version 0.2.0 – Dynamic UI System

**Status:** In Progress    
**Type:** Major Feature Update  

This update focuses on building a flexible and data-driven UI framework.

### 🖼 Planned Features
- Dynamic UI system driven by JSON
- External UI layout definition files
- UI element abstraction layer
- Runtime UI loading and reloading
- Layout and component hierarchy system

### 🛠 Tooling
- External UI editor integration using TiledMap
- JSON-based UI serialization
- Separation between UI logic and layout design

### 🎯 Goals
- Fully data-driven UI architecture
- No hardcoded UI layouts
- Designer-friendly workflow
- Clear separation between engine and presentation layer


--- 

# 🔮 Upcoming Roadmap

## Version 0.3.0 – Rendering Expansion
- Rendering backend improvements
- Resource management system
- Shader abstraction layer
- Texture and asset pipeline foundation

---

## Version 0.4.0 – Input & Interaction Layer
- Unified input abstraction
- Controller support improvements
- Action-mapping system
- Remappable input configuration

## Version 0.5.0 – Networking & Multiplayer Foundation
- Networking layer abstraction
- Dedicated server base
- Packet system
- Synchronization model

## Version 0.6.0 – Modding Infrastructure
- C++ module loader
- Plugin interface
- Public engine API exposure
- Optional scripting integration (Lua research phase)

---

# 🧩 Long-Term Vision

- Fully modular engine core
- Clean API for game layer integration
- Dedicated server ecosystem
- Plugin and mod support
- Cross-platform compatibility
- Optional Vulkan rendering backend
- Performance-first architecture

---

# 📌 Versioning Format

This project follows semantic versioning:

MAJOR.MINOR.PATCH

- MAJOR — Breaking architectural changes
- MINOR — New systems and features
- PATCH — Fixes and minor improvements

---

# 📝 Notes

The early versions of this project focus on building a strong architectural foundation.  
Gameplay features will only be added once core systems are stable, modular, and well-documented.
