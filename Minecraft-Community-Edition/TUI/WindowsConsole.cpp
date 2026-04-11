#include "WindowsConsole.hpp"
#include "Platform.hpp"

#ifdef MCE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include "icon.hpp"

namespace mce::tui {
	BOOL WINAPI ConsoleHandler(DWORD event) {
		return TRUE;
	}

	HICON CreateIconFromSFML(const sf::Image& img) {
		unsigned int width = img.getSize().x;
		unsigned int height = img.getSize().y;

		if (width == 0 || height == 0)
			return nullptr;

		auto pixels = img.getPixelsPtr();

		std::vector<BYTE> bgra(width * height * 4);

		for (unsigned int i = 0; i < width * height; ++i) {
			bgra[i * 4 + 0] = pixels[i * 4 + 2]; // B
			bgra[i * 4 + 1] = pixels[i * 4 + 1]; // G
			bgra[i * 4 + 2] = pixels[i * 4 + 0]; // R
			bgra[i * 4 + 3] = pixels[i * 4 + 3]; // A
		}

		// Describe bitmap
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -((LONG)height); // top-down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* dibPixels = nullptr;

		HBITMAP colorBitmap = CreateDIBSection(
			NULL,
			&bmi,
			DIB_RGB_COLORS,
			&dibPixels,
			NULL,
			0
		);

		if (!colorBitmap)
			return nullptr;

		memcpy(dibPixels, bgra.data(), bgra.size());

		// Create dummy mask (required)
		HBITMAP maskBitmap = CreateBitmap(
			width,
			height,
			1,
			1,
			NULL
		);

		ICONINFO ii = {};
		ii.fIcon = TRUE;
		ii.hbmColor = colorBitmap;
		ii.hbmMask = maskBitmap;

		HICON hIcon = CreateIconIndirect(&ii);

		// Cleanup bitmaps (icon keeps its own copy)
		DeleteObject(colorBitmap);
		DeleteObject(maskBitmap);

		return hIcon;
	}
	sf::Image _icon;
	WindowsConsole::WindowsConsole(core::QEventBus& qBus, sf::WindowHandle window, const std::string& title, uint16_t width, uint16_t height, uint16_t pxlWidth, uint16_t pxlHeight) : 
		qBus(qBus) {
		if (title.empty() && width == 0 && height == 0) {
			if (WindowsConsole::initConsoleWindow()) {
				WindowsConsole::bIsOpen = (bool)WindowsConsole::nwh;
			}
		}
		else if (WindowsConsole::initConsoleWindow()) {
			WindowsConsole::bIsOpen = (bool)WindowsConsole::nwh;
			WindowsConsole::setSize(width, height);

			RECT wndRect, deskRect;

			GetWindowRect(WindowsConsole::nwh, &wndRect);
			GetWindowRect(GetDesktopWindow(), &deskRect);

			int wndWidth = wndRect.right - wndRect.left;
			int wndHeight = wndRect.bottom - wndRect.top;

			int deskWidth = deskRect.right - deskRect.left;
			int deskHeight = deskRect.bottom - deskRect.top;

			int x = (deskWidth - wndWidth) / 2;
			int y = (deskHeight - wndHeight) / 2;

			WindowsConsole::setPosition(x, y);

			WindowsConsole::setTitle(title.c_str());

			SetConsoleCtrlHandler(ConsoleHandler, TRUE);

			_icon.loadFromMemory(icon, ::icon_size);
			HICON hIcon = CreateIconFromSFML(_icon);

			SendMessage(nwh, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(nwh, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		}
	}
	bool WindowsConsole::isOpen() const {
		return WindowsConsole::bIsOpen;
	}
	sf::WindowHandle WindowsConsole::getNativeHandle() const {
		return WindowsConsole::nwh;
	}
	void WindowsConsole::setPosition(const uint16_t x, const uint16_t y) {
		RECT wndRect;
		GetWindowRect(WindowsConsole::getNativeHandle(), &wndRect);
		MoveWindow(WindowsConsole::getNativeHandle(), x, y, wndRect.right, wndRect.bottom, TRUE);
	}
	void WindowsConsole::setSize(const uint16_t width, const uint16_t height) {
		RECT wndRect;
		GetWindowRect(WindowsConsole::getNativeHandle(), &wndRect);
		MoveWindow(WindowsConsole::getNativeHandle(), wndRect.left, wndRect.top, width, height, TRUE);
	}
	sf::Vector2<short> WindowsConsole::getSize() const {
		RECT wndRect;
		GetWindowRect(WindowsConsole::getNativeHandle(), &wndRect);
		return sf::Vector2<short>(wndRect.right, wndRect.bottom);
	}
	sf::Vector2<short> WindowsConsole::getPosition() const {
		RECT wndRect;
		GetWindowRect(WindowsConsole::getNativeHandle(), &wndRect);
		return sf::Vector2<short>(wndRect.left, wndRect.top);
	}
	const std::string WindowsConsole::getTitle() const {
		CHAR buffer[MAX_PATH]{};
		GetWindowTextA(WindowsConsole::getNativeHandle(), buffer, MAX_PATH);
		return buffer;
	}
	void WindowsConsole::setTitle(const char* title) {
		SetWindowTextA(WindowsConsole::getNativeHandle(), title);
	}
	void WindowsConsole::close() {
		this->bIsOpen = false;
	}
	bool WindowsConsole::initConsoleWindow() {
		WindowsConsole::nwh = GetConsoleWindow();
		if (!WindowsConsole::nwh) {
			AllocConsole();
			WindowsConsole::nwh = GetConsoleWindow();
			if (!WindowsConsole::nwh) {
				MessageBoxA(nullptr, "Could Not Init Console Window!", "Error", MB_ICONERROR | MB_OK);
				return false;
			}
		}
		return true;
	}
}

#endif