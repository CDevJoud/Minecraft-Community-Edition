-- -

# XAPI — Cross - Engine Modding ABI

XAPI is a **C ABI - based modding interface** designed for voxel / sandbox - style engines.
It provides a **stable, low - level contract** between engine and mods, enabling binary compatibility across different builds, forks, and even entirely separate engines.

-- -

## ✨ Overview

XAPI is not tied to a specific engine implementation.

Instead, it defines a **strict binary interface(ABI)** that any engine can implement and any mod can target.

This allows:

- Mods compiled once to run across multiple engines
- Different codebases to share the same mod ecosystem
- Language - agnostic bindings via C ABI

-- -

## 🧠 Design Philosophy

- **C ABI first** — maximum portability and language interoperability
- **No engine pointers exposed**— mods operate on handles, not raw memory
- **Explicit lifecycle**— no hidden initialization or runtime magic
- **Event - driven communication**— decoupled interaction via event buses
- **Stable contracts over implementations**— behavior defined by spec, not engine internals

-- -

## 🧩 Core Concepts

### 1. Entry Points

Every XAPI module exports three functions :

```c
XAPI_EXPORT XAPIDescriptor XI_query(Xvoid);
XAPI_EXPORT Xint32 XI_main(Xvoid* pParam);
XAPI_EXPORT Xint32 XI_terminate(Xvoid* pParam);
````

* `XI_query` → returns mod metadata
* `XI_main` → initializes the module
* `XI_terminate` → cleans up resources

-- -

### 2. Device & Context

The engine provides :

* `XIDevice` → resource and system access
* `XIContext` → runtime interaction(events, callbacks)

Mods never construct these manually — they are provided by the engine.

-- -

### 3. Function Table(VTable)

All engine interaction is done through function tables :

```c
device->vtbl->createQEventBus(...)
ctx->vtbl->postEvent(...)
```

This ensures :

* ABI stability
* no direct linking against engine internals

-- -

### 4. Event System

XAPI uses an **event - driven architecture**:

* Mods post events via `postEvent`
* Systems subscribe via `subscribeEvent`
* Communication is decoupled and asynchronous

Example :

```c
XSEventLog log;
log.channel = "default";
log.msg = "Hello World";
log.severity = XE_EVENT_LOG_DEBUG;

ctx->vtbl->postEvent(NULL, &log, XE_EVENT_TYPE_LOG);
```

-- -

### 5. Handles Instead of Pointers

All engine - side objects are represented as **handles**, not pointers:

```c
typedef struct {
	Xint64 idx;
} XHQEventBus;
```

This ensures :

* memory safety across modules
* engine - side ownership control
* compatibility across implementations

-- -

## 🔄 Module Lifecycle

1. Engine loads module(`.dll`)
	2. Calls `XI_query` → retrieves metadata
	3. Calls `XI_main`

	* provides function pointer to create device / context
	* mod initializes and registers callbacks
	4. Engine drives execution via callbacks :

* `onInit`
* `onUpdate`
* `onShutdown`
5. Engine calls `XI_terminate` on unload

-- -

## 🧱 CXAPI(C Wrapper)

CXAPI is a thin helper layer built on top of XAPI to simplify usage in C.

It provides :

* global state management
* callback registration
* utility functions(logging, event posting)

Example:

```c
CXAPI_setOnInitCB(onInit);
CXAPI_setOnUpdateCB(onUpdate);
CXAPI_setOnShutdownCB(onShutdown);
```

-- -

## 🌍 Multi - Language Support

Because XAPI is pure C ABI, it can be used from many languages.

Bindings can be created by the community for:

* C / C++ (native)
* Rust
* Zig
* Nim
* Go
* C# (AOT)
* Python(via C API / Cython)
* JavaScript / Lua(via embedded runtimes)

The core project focuses on :

> **C/C++ reference implementation and wrappers**

-- -

## 🔐 Safety Model

* Mods never receive raw engine pointers
* All access goes through validated interfaces
* Engine retains full ownership of resources
* Invalid operations return error codes(`XE_ERROR`)

-- -

## 🔢 Versioning

XAPI uses explicit versioning:

```c
#define XAPI_VERSION 1
```

Future versions must maintain :

* backward compatibility where possible
* strict ABI guarantees

-- -

## 🎯 Goals

* Provide a **portable modding standard**
* Enable **cross - engine compatibility**
* Keep the core **minimal, stable, and predictable**
* Allow higher - level systems to be built on top

-- -

## 🚀 Example(Minimal Mod)

```c
//Globals
XAPIDescriptor desc;
XIExports exports;
XHQEventBus qBus;
XI_createDeviceAndContextFn XI_createDeviceAndContext;
XI_destroyDeviceAndContextFn XI_destroyDeviceAndContext;

XAPI_LOCAL Xint32 onUpdate(Xvoid) {
	return 1;
}

XAPI_LOCAL Xint32 onInit(Xvoid) {
	return 1;
}

XAPI_LOCAL Xint32 onShutdown(Xvoid) {
	return 1;
}

XAPI_EXPORT Xconst XAPIDescriptor XI_query(Xvoid) {
	XAPIDescriptor desc;
	desc.author = "CDevJoud";
	desc.language = "C";
	desc.name = "My Mod!";
	desc.version = 1;
	desc.sdkName = "CXAPI";
	return desc;
}

XAPI_EXPORT Xint32 XI_main(Xvoid* pParam) {
	XI_createDeviceAndContext = (XI_createDeviceAndContextFn)pParam;
	Xconst Xint32 ret = XI_createDeviceAndContext(&device, &ctx);
	
	if (ret != XE_ERROR) {
		// request for the APP QEventBus
		if (device->vtbl->createQEventBus != XAPI_NULL) {
			XQEventBusDescriptor qBusdesc;
			qBusdesc.name = "APP";
			qBusdesc.maxSize = 0xFFFF;
			Xint32 res = device->vtbl->createQEventBus(device, &qBus, qBusdesc);
		}
		else {
			return XE_ERROR; //Fatal Error!
		}
		
		exports.onInit = onInit;
		exports.onShutdown = onShutdown;
		exports.onUpdate = onUpdate;

		if (device->vtbl->setXIExports != XAPI_NULL) {
			device->vtbl->setXIExports(&device, exports);
		}
		else {
			return XE_ERROR; //Fatal Error!
		}
	}
	XSEventLog log;
	log.channel = "default";
	log.msg = "Running XAPI!";
	log.severity = XE_EVENT_LOG_DEBUG;
	ctx->vtbl->postEvent(&qBus, &log, XE_EVENT_TYPE_LOG);
	return ret != XE_ERROR;
}

XAPI_EXPORT Xint32 XI_terminate(Xvoid* pParam) {
	XI_destroyDeviceAndContext = (XI_destroyDeviceAndContextFn)pParam;
	return XI_destroyDeviceAndContext(&device, &ctx) != XE_ERROR;
}
```

---

## 📌 Summary

XAPI is :

* a **specification**, not just an implementation
* a **binary contract**, not a framework
* a **foundation layer** for building mod ecosystems

---

**XAPI is an open specification. Anyone is free to implement it in their own engine or create language bindings.**

## 📄 License

MIT License

Copyright (c) 2026 Joud Kandeel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---