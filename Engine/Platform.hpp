#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define MCE_PLATFORM_WINDOWS
#define PLATFORM_NAME "windows"
#else
#error "x86 is not supported"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#define MCE_PLATFORM_MACOS
#elif defined(__linux__)
#define MCE_PLATFORM_LINUX
#else
#error "Unsupported platform"
#endif


