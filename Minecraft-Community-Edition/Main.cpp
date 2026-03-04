#include "Common/Platform.hpp"
#include "Minecraft.hpp"

#if defined(MCE_PLATFORM_WINDOWS) && defined(NDEBUG)
#include <minwindef.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	MCE::Minecraft minecraft;
	minecraft.run();

	return 0;
}
