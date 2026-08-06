#include "EventProcessor.hpp"
#include "Platform.hpp"
#include <Core/WindowBase.hpp>
#include <Core/Event.hpp>

#ifdef MCE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace dms::tui {
	EventProcessor::EventProcessor() : hInput(INVALID_HANDLE_VALUE), nwh(nullptr) {
	
	}
	bool EventProcessor::initEventProcessor(void* hConsoleIn) {
		EventProcessor::hInput = hConsoleIn;
		return false;
	}

	bool EventProcessor::initEventProcessor(sf::WindowHandle nwh) {
		return false;
	}

	void EventProcessor::processEvents() {
		if (hInput != INVALID_HANDLE_VALUE) {
#ifdef MCE_PLATFORM_WINDOWS
			for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
				newKeyboardCondition[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));

				keyboardCondition[i].bStrokePressed = false;
				keyboardCondition[i].bStrokeReleased = false;
				if (newKeyboardCondition[i] != oldKeyboardCondition[i]) {
					if (newKeyboardCondition[i]) {
						keyboardCondition[i].bStrokePressed = !keyboardCondition[i].bStrokeIsHeld;
						keyboardCondition[i].bStrokeIsHeld = true;
					}
					else {
						keyboardCondition[i].bStrokeReleased = true;
						keyboardCondition[i].bStrokeIsHeld = false;
					}
				}
				oldKeyboardCondition[i] = newKeyboardCondition[i];
			}

			SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS);

			INPUT_RECORD inBuf[32]{};
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(hInput, &events);
			if (events > 0) {
				ReadConsoleInput(hInput, (PINPUT_RECORD)&inBuf, events, &events);
			}
			
			for (DWORD i = 0; i < events; i++) {
				switch (inBuf[i].EventType) {
				case MOUSE_EVENT:
				{
					switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
					case MOUSE_MOVED:
					{
						mousePos.x = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						mousePos.y = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}
					break;

					case 0:
					{
						for (int m = 0; m < 5; m++)
							newMouseCondition[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
					}
					break;

					default:
						break;
					}
				}
				break;
				
				default:
					break;

				}
			}
			for (int m = 0; m < 5; m++) {
				mouseCondition[m].bStrokePressed = false;
				mouseCondition[m].bStrokeReleased = false;

				if (newMouseCondition[m] != oldMouseCondition[m]) {
					if (newMouseCondition[m]) {
						mouseCondition[m].bStrokePressed = true;
						mouseCondition[m].bStrokeIsHeld = true;
					}
					else {
						mouseCondition[m].bStrokeReleased = true;
						mouseCondition[m].bStrokeIsHeld = false;
					}
				}

				oldMouseCondition[m] = newMouseCondition[m];
			}

#endif
		}
		else if (nwh != nullptr) {
			memcpy(oldKeyboardCondition, newKeyboardCondition, sizeof(newKeyboardCondition));

			sf::WindowBase window(nwh);
			for (sf::Event event{}; window.pollEvent(event);) {
				if (event.type == sf::Event::KeyPressed) {
					newKeyboardCondition[event.key.code] = true;
				}
				if (event.type == sf::Event::KeyReleased) {
					newKeyboardCondition[event.key.code] = false;
				}
				if (event.type == sf::Event::MouseButtonPressed) {
					newMouseCondition[event.mouseButton.button] = true;
				}
				if (event.type == sf::Event::MouseButtonReleased) {
					newMouseCondition[event.mouseButton.button] = false;
				}
			}
			for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
				keyboardCondition[i].bStrokePressed = (newKeyboardCondition[i] && !oldKeyboardCondition[i]);
				keyboardCondition[i].bStrokeReleased = (!newKeyboardCondition[i] && oldKeyboardCondition[i]);
				keyboardCondition[i].bStrokeIsHeld = newKeyboardCondition[i];
			}
			for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
				mouseCondition[i].bStrokePressed = (newMouseCondition[i] && !oldMouseCondition[i]);
				mouseCondition[i].bStrokeReleased = (!newMouseCondition[i] && oldMouseCondition[i]);
				mouseCondition[i].bStrokeIsHeld = newMouseCondition[i];
			}
		}
	}

	EventProcessor::KeyStrokeCondition EventProcessor::keyboard(sf::Keyboard::Key key) {
		return keyboardCondition[key];
	}

	EventProcessor::KeyStrokeCondition EventProcessor::Mouse(sf::Mouse::Button btn) {
		return mouseCondition[btn];
	}

	sf::Vector2<short> EventProcessor::getMousePos() const {
		return mousePos;
	}

}

