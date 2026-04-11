#pragma once
#include "IConsole.hpp"

namespace mce::tui {
	class WindowsConsole : public IConsole {
	public:

		WindowsConsole(core::QEventBus& qBus, sf::WindowHandle window, const std::string& title, uint16_t width, uint16_t height, uint16_t pxlWidth, uint16_t pxlHeight);

		virtual bool isOpen() const override;
		virtual sf::WindowHandle getNativeHandle() const override;
		virtual void setPosition(const uint16_t x, const uint16_t y) override;
		virtual void setSize(const uint16_t width, const uint16_t height) override;

		virtual sf::Vector2<short> getSize() const override;
		virtual sf::Vector2<short> getPosition() const override;

		virtual const std::string getTitle() const override;

		virtual void setTitle(const char const* title) override;

		virtual void close() override;

		virtual bool initConsoleWindow() override;

	private:
		bool bIsOpen;
		sf::WindowHandle nwh;
		core::QEventBus& qBus;
	};
}
