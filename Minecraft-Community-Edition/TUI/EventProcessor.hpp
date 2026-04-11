#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowHandle.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace mce::tui {
	class EventProcessor {
	private:
		struct KeyStrokeCondition {
			bool bStrokePressed : 1, bStrokeReleased : 1, bStrokeIsHeld : 1;
		};
	public:
		EventProcessor();

		bool initEventProcessor(void* hConsoleIn);

		bool initEventProcessor(sf::WindowHandle nwh);

		void processEvents();

		KeyStrokeCondition keyboard(sf::Keyboard::Key key);
		KeyStrokeCondition Mouse(sf::Mouse::Button btn);

		sf::Vector2<short> getMousePos() const;
	private:
		friend class Panel;
		KeyStrokeCondition keyboardCondition[sf::Keyboard::KeyCount];
		KeyStrokeCondition mouseCondition[sf::Mouse::ButtonCount];
		bool newKeyboardCondition[sf::Keyboard::KeyCount];
		bool oldKeyboardCondition[sf::Keyboard::KeyCount];
		bool newMouseCondition[sf::Mouse::ButtonCount];
		bool oldMouseCondition[sf::Mouse::ButtonCount];

		sf::Vector2<short> mousePos;
		void* hInput;

		sf::WindowHandle nwh;
	};
}


