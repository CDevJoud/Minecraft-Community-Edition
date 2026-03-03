# Minecraft-Community-Edition

![Project Logo](logo.webp)

## Cross Platform Support (Linux, macOS, etc) and Rewriting, Restructuring, and Redocumenting the LCEMP

After spending time analyzing the game’s source code, I noticed that much of it is poorly structured, inconsistently maintained, and lacks proper documentation. Because of this, I have decided to rebuild the project from the ground up.

The new project will be called **Minecraft Community Edition**, and it will be a complete rewrite using modern C++ with a cleaner architecture, better structure, and proper documentation throughout the codebase. The goal is to create a stronger technical foundation that is easier to maintain, extend, and optimize over time.

### Independent Codebase

Minecraft Community Edition will not directly depend on the current LCEMP source code. Instead, it will follow its own coding style, structure, and design philosophy. The new codebase will be modular, readable, and future-proof.

One of the main improvements will be performance. Since the current version runs on x86 architecture, which limits performance on modern systems, the project will migrate to x64 in order to improve memory handling and overall efficiency.

### Cross-Platform Support

Cross-platform support is an important objective. The new project is planned to support:

- Linux  
- macOS  
- Windows  

This will make the project more accessible and flexible for different users and developers.

### Modding Support

Another major focus is modding support. The plan is to implement a C++ mod loader that allows developers to create modifications without directly interacting with the internal engine code.

If this approach proves stable and practical, Lua scripting support will also be introduced to make modding more accessible for developers who are not comfortable working with C++.

### Multiplayer and Server Infrastructure

Multiplayer will remain a priority. The plan includes:

- Developing an improved server protocol  
- Creating dedicated server software for private and public hosting  
- Supporting plugin development similar to large Minecraft server ecosystems  

This will give server owners flexibility and greater control over their servers.

### Rendering Technology

The engine will initially use OpenGL for rendering. Once the project reaches a stable state, a transition to Vulkan may be considered to further enhance rendering performance and efficiency.

---

[Read the code style guide](CODE_STYLE.md)
