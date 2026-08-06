
class Startup {
public:
	Startup() = default;
	virtual int run() = 0;
};

#include "Platform.hpp"
#if defined(MCE_PLATFORM_WINDOWS) && defined(NDEBUG)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#define MCE_STARTUP(_Startup)                                      							 \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,       						 \
                   LPSTR lpCmdLine, int nCmdShow)                      						 \
{                                                                      						 \
	LPWSTR pCmdLine = GetCommandLineW();													 \
																							 \
	int argc = 0;																			 \
	LPWSTR* argvW = CommandLineToArgvW(pCmdLine, &argc);									 \
																							 \
	if (argvW == NULL)																		 \
		return 1;																			 \
																							 \
	char** argv = (char**)LocalAlloc(LPTR, argc * sizeof(char*));							 \
																							 \
	for (int i = 0; i < argc; ++i) {														 \
		int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);		 \
																							 \
		argv[i] = (char*)LocalAlloc(LPTR, size);											 \
																							 \
		if (argv[i]) {																		 \
			WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], size, NULL, NULL);		 \
		}																					 \
		else {																				 \
			argv[i] = NULL;     													         \
		}																					 \
	}																						 \
																							 \
	_Startup app(argc, argv);																 \
																							 \
	for (int i = 0; i < argc; ++i) {														 \
		LocalFree(argv[i]);																	 \
	}																						 \
	LocalFree(argv);																		 \
																							 \
	LocalFree(argvW);																		 \
																							 \
	int nExitCode = app.run();																		 \
	TerminateProcess(GetCurrentProcess(), nExitCode); \
}
#else

#define MCE_STARTUP(_Startup) \
int main(int argc, char* argv[]) { \
	int nExitCode = 0x00; { \
		_Startup app(argc, argv); \
		nExitCode = app.run();\
	}\
	ExitProcess(nExitCode);\
}
#endif

