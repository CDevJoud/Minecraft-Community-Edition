#pragma once
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/Types.h>

#include <Core/WindowBase.hpp>
#include <Core/Event.hpp>
#include <Core/Clock.hpp>
#include <Core/QEventBus.hpp>

namespace mce::ui::priv {
	class SystemInterface_dms : public Rml::SystemInterface {
	public:
		SystemInterface_dms(core::QEventBus& qBus);
		~SystemInterface_dms();

		void setWindow(sf::WindowBase* window);

		double GetElapsedTime() override;

		void SetMouseCursor(const Rml::String& cursorName) override;
		
		void SetClipboardText(const Rml::String& text) override;
		void GetClipboardText(Rml::String& text) override;

		bool LogMessage(Rml::Log::Type type, const Rml::String& msg) override;

	private:
		
		sf::WindowBase* window;
		core::QEventBus& qBus;
		sf::Clock clock;
		sf::Cursor cursor_default;
		sf::Cursor cursor_move;
		sf::Cursor cursor_pointer;
		sf::Cursor cursor_resize;
		sf::Cursor cursor_cross;
		sf::Cursor cursor_text;
		sf::Cursor cursor_unavailable;
	};
	bool inputEventHandler(Rml::Context* ctx, sf::WindowHandle hWnd, sf::Event& event);

	Rml::Input::KeyIdentifier convertKey(sf::Keyboard::Key key);

	int getKeyModifierState();
}
