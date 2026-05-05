#pragma once
#include <Core/QEventBus.hpp>
#include <Core/WindowHandle.hpp>

namespace mce::tui {
	class IConsole {
	public:
		//IConsole(core::QEventBus& qBus, sf::WindowHandle window, const std::string& title, uint16_t width, uint16_t height, uint16_t pxlWidth, uint16_t pxlHeight);
		
		virtual bool isOpen() const = 0;
		virtual sf::WindowHandle getNativeHandle() const = 0;
		virtual void setPosition(const uint16_t x, const uint16_t y) = 0;
		virtual void setSize(const uint16_t width, const uint16_t height) = 0;

		virtual sf::Vector2<short> getSize() const = 0;
		virtual sf::Vector2<short> getPosition() const = 0;

		virtual const std::string getTitle() const = 0;

		virtual void setTitle(const char const* title) = 0;

		virtual void close() = 0;
		
		virtual bool initConsoleWindow() = 0;

	};
}

