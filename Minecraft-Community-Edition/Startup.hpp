
class Startup {
public:
	virtual int run() = 0;
};

#include "Common/Platform.hpp"
#if defined(MCE_PLATFORM_WINDOWS) && defined(NDEBUG)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define MCE_STARTUP(Startup) int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {Startup app; return app.run();}
#else
#define MCE_STARTUP(Startup) int main() {Startup app; return app.run();}
#endif
