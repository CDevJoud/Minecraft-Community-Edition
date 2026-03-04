#pragma once

#include "Platform.hpp"

#if defined(MCE_PLATFORM_WINDOWS) && defined(NDEBUG)
#include <minwindef.h>
#define MCE_ENTRYPOINT int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
#define MCE_ENTRYPOINT int main()
#endif