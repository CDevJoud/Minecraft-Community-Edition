# C++ Code Style Guide

This document defines the official coding conventions for this project.  
The goal is consistency, readability, maintainability, and performance.

---

# 1. Core Principles

- Prefer clarity to cleverness.
- Prefer data-oriented design where appropriate.
- Avoid unnecessary abstraction layers.
- Reduce boilerplate, especially trivial getters/setters.
- Write modern C++ (C++20 or newer).
- Keep systems modular and engine-agnostic.
- Consistency is more important than personal preference.

---

# 2. Naming Conventions

## 2.1 Types (Classes, Structs, Enums)

- Use **PascalCase**.
- Do not use prefixes like `C_`, `E`, or `I`.

```cpp
class InputManager;
struct ImageData;
enum class KeyboardMode;
```
---

## 2.2 Functions

* Use **camelCase**.
* Start with a verb.
* Be descriptive.

```cpp
void initialize();
bool isConnected();
void setDeadzone(float value);
ImageData& getImageData();
```

---

## 2.3 Variables

* Use **camelCase**.
* Be descriptive and meaningful.

```cpp
int playerHealth;
float movementSpeed;
bool isActive;
```

---

## 2.4 Constants

* Use `constexpr`.
* Use **UPPER_CASE**.

```cpp
constexpr int MAX_PLAYERS = 4;
constexpr float DEFAULT_DEADZONE = 0.15f;
```

---

## 2.5 Member Variables

* Use camelCase

```cpp
class Image {
private:
    Vector2 size;
    std::string fileName;
};
```

---

# 3. Formatting

## 3.1 Indentation

* 1 tab per level.
* Do not use spaces.

## 3.2 Braces

* **K&R style**: opening braces on the same line.

```cpp
if (condition) {
    doSomething();
}

class Player {
public:
    void update();
};
```

## 3.3 Line Length

* Maximum 100–120 characters.

---

# 4. Avoid Excessive Getters and Setters

Do not create trivial getters/setters for simple member access.

### Avoid

```cpp
std::string getFileName();
int getImageSizeX();
int getImageSizeY();
```

### Prefer Structured Data Access

```cpp
Image& imgInfo = img.getImageInfo();
imgInfo.size.x;
imgInfo.size.y;
```

* Encapsulation should protect invariants, not hide trivial data.
* Prefer returning references to structured data when possible.

---

# 5. Modern C++ Rules

## 5.1 Enum Classes

```cpp
enum class KeyboardMode {
    Default,
    Numeric,
    Password
};
```

## 5.2 `constexpr` over `#define`

❌ Avoid:

```cpp
#define MAX_TEXTURES 128
```

✅ Prefer:

```cpp
constexpr int MAX_TEXTURES = 128;
```

## 5.3 `nullptr`

```cpp
MyClass* ptr = nullptr;
```

## 5.4 Smart Pointers

```cpp
eastl::unique_ptr<Texture> texture;
eastl::shared_ptr<Model> model;
```

* Avoid raw `new` and `delete` if it is possible.

---

# 6. Header Rules

* Use `#pragma once`.
* Keep headers minimal.
* Forward declare when possible.
* One primary class per file.
* Avoid unnecessary includes.

```cpp
#pragma once

class Renderer;
```

---

# 7. Fully Qualified Method Definitions and Calls

To improve clarity in inheritance chains:

## 7.1 Method Definitions

All method definitions must use the class scope.

```cpp
class Window {
public:
    void setPosition();
};
```

```cpp
void Window::setPosition() {
    // implementation
}
```

---

## 7.2 Inherited Classes

Always define overridden methods with the derived class name.

```cpp
class Console : public Window {
public:
    void setPosition() override;
};
```

```cpp
void Console::setPosition() {
    // implementation
}
```

---

## 7.3 Fully Qualified Calls Across Inheritance

When calling base-class methods from a derived class, use the full inheritance chain.

```cpp
class Window {
public:
    void setPosition();
};

class Console : public Window {
};

class Application : public Console {
public:
    Application();
};
```

```cpp
Application::Application() {
    Application::Console::Window::setPosition();
}
```

* Makes it clear which method is being invoked.
* Recommended for deep inheritance hierarchies.

---

# 8. Inheritance Rules

* Use inheritance only for true "is-a" relationships.
* Prefer composition to inheritance.
* Always mark overridden functions with `override`.
* Avoid deep or ambiguous inheritance hierarchies.

---

<!--
# 9. Error Handling

* Do not return magic numbers.
* Use `std::optional` when a value may be absent.
* Use exceptions only for exceptional cases.
* Prefer explicit error handling.

```cpp
std::optional<Player> findPlayer(int id);
```
-->

---

# 9. Comments

* Explain **why**, not **what**.

❌ Bad:

```cpp
// increment i
i++;
```

✅ Good:

```cpp
// Skip metadata header element
i++;
```

---

# 10. Platform Independence

* Avoid Windows types (`DWORD`, `HRESULT`, etc.) in core modules.
* Avoid OS-specific APIs in core logic.
* Use fixed-width integer types.

```cpp
uint32_t
int64_t
std::u16string
```

* Platform-specific code should be isolated.

---

# 11. Performance Guidelines

* Pass large objects by `const&`.
* Move objects when transferring ownership.
* Reserve container capacity when known.
* Avoid unnecessary heap allocations.
* Prefer stack allocation when possible.
* Avoid hidden copies.

---

# 12. File Organization

* One main class per file:

```text
Image.hpp
Image.cpp
```

---

# 13. Documentation

* Public APIs must use Doxygen-style comments.

```cpp
/// Sets the controller deadzone.
/// @param value Deadzone between 0.0 and 1.0
void setDeadzone(float value);
```

---

# 15. Summary

* Consistency is more important than personal preference.
* Trivial getters/setters should be avoided; structured data access is preferred.
* Fully qualified method calls in inheritance chains improve clarity.
* All new code must follow this style guide.
