#include "Console.hpp"
#include "WindowsConsole.hpp"

#ifdef MCE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Panel.hpp"
#endif

namespace mce::tui {
	
	Console::Console(core::QEventBus& qBus, sf::WindowHandle window, const std::string& title, uint16_t width, uint16_t height, uint16_t pxlWidth, uint16_t pxlHeight, Type type) :
		type(type), 
		interface(nullptr) {
		if (Console::type == Console::Type::NativeWindows) {
			Console::interface = new WindowsConsole(qBus, window, title, width * pxlWidth, height * pxlHeight, pxlWidth, pxlHeight);
#ifdef MCE_PLATFORM_WINDOWS
			if (Console::interface->isOpen()) {
				Console::hInput = GetStdHandle(STD_INPUT_HANDLE);
				Console::hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

				SMALL_RECT rect = { 0, 0, 1, 1 };
				SetConsoleWindowInfo(Console::getOutputHandle(), TRUE, &rect);

				if (!SetConsoleScreenBufferSize(Console::getOutputHandle(), { (short)width, (short)height })) {
					MessageBoxA(nullptr, "Couldn't set the console screen buffer size!", "Error", MB_ICONERROR | MB_OK);
				}

				SetConsoleActiveScreenBuffer(Console::getOutputHandle());

				CONSOLE_FONT_INFOEX cfi{};
				cfi.cbSize = sizeof(cfi);
				cfi.nFont = 0;
				cfi.dwFontSize.X = pxlWidth;
				cfi.dwFontSize.Y = pxlHeight;
				cfi.FontFamily = FF_DONTCARE;
				cfi.FontWeight = FW_NORMAL;

				wcscpy_s(cfi.FaceName, L"Consolas");
				SetCurrentConsoleFontEx(Console::getOutputHandle(), FALSE, &cfi);

				Console::viewSpace.left = 0;
				Console::viewSpace.top = 0;
				Console::viewSpace.width = width - 1;
				Console::viewSpace.height = height - 1;
				if (!SetConsoleWindowInfo(Console::getOutputHandle(), TRUE, reinterpret_cast<PSMALL_RECT>(&viewSpace))) {
					// we force the size
					Console::getInterface()->setSize(width * pxlWidth, height * pxlHeight);
				}

				SetConsoleMode(Console::getInputHandle(), ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

				Console::re.hConsole = Console::getOutputHandle();
				Console::re.viewSpace = Console::viewSpace;
				Console::re.buffer.resize(Console::viewSpace.width * Console::viewSpace.height, CharInfo{});
				Console::initEventProcessor(hInput);
			}
#endif
		}
	}
	Console::~Console() {
		if (Console::interface) {
			delete Console::interface;
			Console::interface = nullptr;
		}
	}
	IConsole* Console::getInterface() {
		return Console::interface;
	}
	void* Console::getInputHandle() {
		return Console::hInput;
	}

	void* Console::getOutputHandle() {
		return Console::hOutput;
	}
	bool Console::insertComponent(const eastl::shared_ptr<Component>& component) {
		Console::components.push_back(component);
		component->onInit();
		return true;
	}
	void Console::display() {
		Console::processEvents();

		for (int i = 0; i < Console::components.size(); i++) {
			auto& component = Console::components[i];
			component->onUpdate(this);

			/*if (components->type == Component::Type::Panel && components->bTargeted) {
				
			}*/

			component->onRender(this);
		}

#ifdef MCE_PLATFORM_WINDOWS
		SMALL_RECT rect = { 0, 0, Console::viewSpace.width, Console::viewSpace.height };
		WriteConsoleOutputW(Console::getOutputHandle(), (PCHAR_INFO)Console::re.buffer.data(), { Console::re.viewSpace.width, Console::re.viewSpace.height }, {}, &rect);
#endif
	}
	bool Console::isOpen() {
		return Console::interface->isOpen();
	}
	void Console::close() {
		Console::interface->close();
	}
}
