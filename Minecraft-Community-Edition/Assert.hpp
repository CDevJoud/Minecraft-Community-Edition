#pragma once

#include "IO/Logger.hpp"
#include "Platform.hpp"

#ifndef NDEBUG
#ifdef MCE_PLATFORM_WINDOWS
#define MCE_DEBUGBREAK() __debugbreak()
#elif defined(MCE_PLATFORM_MACOS) || defined(MCE_PLATFORM_LINUX)
#include <signal.h>
#define MCE_DEBUGBREAK() raise(SIGTRAP)
#endif

#define MCE_ASSERT(check, ...) { if (!(check)) { MCE_ERROR(__VA_ARGS__); MCE_DEBUGBREAK(); } }
#else
#define MCE_DEBUGBREAK()
#define MCE_ASSERT(...)
#endif