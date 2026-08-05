#include "RmlUI_Platform_dms.hpp"
#include <RmlUi/Core/Context.h>

#include <RmlUi/Core/StringUtilities.h>
#include <Core/WindowBase.hpp>
#include <Core/Clipboard.hpp>

#include <IO/Stream/MemoryInputStream.hpp>

namespace mce::ui::priv {
	SystemInterface_dms::SystemInterface_dms(core::QEventBus& qBus) : qBus(qBus) {
		SystemInterface_dms::clock.restart();

		bool cursors_valid = true;
		cursors_valid &= cursor_default.loadFromSystem(sf::Cursor::Arrow);
		cursors_valid &= cursor_move.loadFromSystem(sf::Cursor::SizeAll);
		cursors_valid &= cursor_pointer.loadFromSystem(sf::Cursor::Hand);
		cursors_valid &= cursor_resize.loadFromSystem(sf::Cursor::SizeTopLeftBottomRight) || cursor_resize.loadFromSystem(sf::Cursor::SizeAll);
		cursors_valid &= cursor_cross.loadFromSystem(sf::Cursor::Cross);
		cursors_valid &= cursor_text.loadFromSystem(sf::Cursor::Text);
		cursors_valid &= cursor_unavailable.loadFromSystem(sf::Cursor::NotAllowed);
	}
	SystemInterface_dms::~SystemInterface_dms() {

	}
	void SystemInterface_dms::setWindow(sf::WindowBase* window) {
		SystemInterface_dms::window = window;
	}
	double SystemInterface_dms::GetElapsedTime() {
		return SystemInterface_dms::clock.getElapsedTime().asSeconds();
	}
	// TODO we need a way to pass the window base to the SystemInterface_dms
	void SystemInterface_dms::SetMouseCursor(const Rml::String& cursorName) {
		if (cursorName.empty() || cursorName == "arrow") {
			window->setMouseCursor(cursor_default);
		}
		else if (cursorName == "move") {
			window->setMouseCursor(cursor_move);
		}
		else if (cursorName == "pointer") {
			window->setMouseCursor(cursor_pointer);
		}
		else if (cursorName == "resize") {
			window->setMouseCursor(cursor_resize);
		}
		else if (cursorName == "cross") {
			window->setMouseCursor(cursor_cross);
		}
		else if (cursorName == "text") {
			window->setMouseCursor(cursor_text);
		}
		else if (cursorName == "unavailable") {
			window->setMouseCursor(cursor_unavailable);
		}
		else if (Rml::StringUtilities::StartsWith(cursorName, "rmlui-scroll")) {
			window->setMouseCursor(cursor_move);
		}
	}
	// TODO we need a way to pass the window base to the SystemInterface_dms
	void SystemInterface_dms::SetClipboardText(const Rml::String& text) {
		sf::Clipboard::setString(text);
	}
	// TODO we need a way to pass the window base to the SystemInterface_dms
	void SystemInterface_dms::GetClipboardText(Rml::String& text) {
		text = sf::Clipboard::getString();
	}
	bool SystemInterface_dms::LogMessage(Rml::Log::Type type, const Rml::String& msg) {
		event::Log log;
		log.channel = "default";
		switch (type) {
		case Rml::Log::LT_ERROR:
			log.severity = event::Log::Severity::ERROR;
			break;
		case Rml::Log::LT_WARNING:
			log.severity = event::Log::Severity::WARN;
			break;
		case Rml::Log::LT_INFO:
			log.severity = event::Log::Severity::INFO;
			break;
		case Rml::Log::LT_DEBUG:
			log.severity = event::Log::Severity::DEBUG;
			break;
		default:
			log.severity = event::Log::Severity::INFO;
		}

		log.msg += "[RMLUI] ";
		log.msg += msg;
		qBus.post(log);

		return true;
	}
	bool inputEventHandler(Rml::Context* context, sf::WindowHandle hWnd, sf::Event& event) {
		bool result = true;
		
		switch (event.type) {
		case sf::Event::MouseMoved: result = context->ProcessMouseMove(event.mouseMove.x, event.mouseMove.y, getKeyModifierState()); break;
		case sf::Event::MouseButtonPressed: result = context->ProcessMouseButtonDown(event.mouseButton.button, getKeyModifierState()); break;
		case sf::Event::MouseButtonReleased: result = context->ProcessMouseButtonUp(event.mouseButton.button, getKeyModifierState()); break;
		case sf::Event::MouseWheelMoved: result = context->ProcessMouseWheel(float(-event.mouseWheel.delta), getKeyModifierState()); break;
		case sf::Event::MouseLeft: result = context->ProcessMouseLeave(); break;
		case sf::Event::TextEntered:
		{
			Rml::Character character = Rml::Character(event.text.unicode);
			if (character == Rml::Character('\r'))
				character = Rml::Character('\n');

			if (event.text.unicode >= 32 || character == Rml::Character('\n'))
				result = context->ProcessTextInput(character);
		}
		break;
		case sf::Event::KeyPressed: result = context->ProcessKeyDown(convertKey(event.key.code), getKeyModifierState()); break;
		case sf::Event::KeyReleased: result = context->ProcessKeyUp(convertKey(event.key.code), getKeyModifierState()); break;
		default: break;
		}
		return result;
	}
	Rml::Input::KeyIdentifier convertKey(sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::Key::A:         return Rml::Input::KI_A;
		case sf::Keyboard::Key::B:         return Rml::Input::KI_B;
		case sf::Keyboard::Key::C:         return Rml::Input::KI_C;
		case sf::Keyboard::Key::D:         return Rml::Input::KI_D;
		case sf::Keyboard::Key::E:         return Rml::Input::KI_E;
		case sf::Keyboard::Key::F:         return Rml::Input::KI_F;
		case sf::Keyboard::Key::G:         return Rml::Input::KI_G;
		case sf::Keyboard::Key::H:         return Rml::Input::KI_H;
		case sf::Keyboard::Key::I:         return Rml::Input::KI_I;
		case sf::Keyboard::Key::J:         return Rml::Input::KI_J;
		case sf::Keyboard::Key::K:         return Rml::Input::KI_K;
		case sf::Keyboard::Key::L:         return Rml::Input::KI_L;
		case sf::Keyboard::Key::M:         return Rml::Input::KI_M;
		case sf::Keyboard::Key::N:         return Rml::Input::KI_N;
		case sf::Keyboard::Key::O:         return Rml::Input::KI_O;
		case sf::Keyboard::Key::P:         return Rml::Input::KI_P;
		case sf::Keyboard::Key::Q:         return Rml::Input::KI_Q;
		case sf::Keyboard::Key::R:         return Rml::Input::KI_R;
		case sf::Keyboard::Key::S:         return Rml::Input::KI_S;
		case sf::Keyboard::Key::T:         return Rml::Input::KI_T;
		case sf::Keyboard::Key::U:         return Rml::Input::KI_U;
		case sf::Keyboard::Key::V:         return Rml::Input::KI_V;
		case sf::Keyboard::Key::W:         return Rml::Input::KI_W;
		case sf::Keyboard::Key::X:         return Rml::Input::KI_X;
		case sf::Keyboard::Key::Y:         return Rml::Input::KI_Y;
		case sf::Keyboard::Key::Z:         return Rml::Input::KI_Z;
		case sf::Keyboard::Key::Num0:      return Rml::Input::KI_0;
		case sf::Keyboard::Key::Num1:      return Rml::Input::KI_1;
		case sf::Keyboard::Key::Num2:      return Rml::Input::KI_2;
		case sf::Keyboard::Key::Num3:      return Rml::Input::KI_3;
		case sf::Keyboard::Key::Num4:      return Rml::Input::KI_4;
		case sf::Keyboard::Key::Num5:      return Rml::Input::KI_5;
		case sf::Keyboard::Key::Num6:      return Rml::Input::KI_6;
		case sf::Keyboard::Key::Num7:      return Rml::Input::KI_7;
		case sf::Keyboard::Key::Num8:      return Rml::Input::KI_8;
		case sf::Keyboard::Key::Num9:      return Rml::Input::KI_9;
		case sf::Keyboard::Key::Numpad0:   return Rml::Input::KI_NUMPAD0;
		case sf::Keyboard::Key::Numpad1:   return Rml::Input::KI_NUMPAD1;
		case sf::Keyboard::Key::Numpad2:   return Rml::Input::KI_NUMPAD2;
		case sf::Keyboard::Key::Numpad3:   return Rml::Input::KI_NUMPAD3;
		case sf::Keyboard::Key::Numpad4:   return Rml::Input::KI_NUMPAD4;
		case sf::Keyboard::Key::Numpad5:   return Rml::Input::KI_NUMPAD5;
		case sf::Keyboard::Key::Numpad6:   return Rml::Input::KI_NUMPAD6;
		case sf::Keyboard::Key::Numpad7:   return Rml::Input::KI_NUMPAD7;
		case sf::Keyboard::Key::Numpad8:   return Rml::Input::KI_NUMPAD8;
		case sf::Keyboard::Key::Numpad9:   return Rml::Input::KI_NUMPAD9;
		case sf::Keyboard::Key::Left:      return Rml::Input::KI_LEFT;
		case sf::Keyboard::Key::Right:     return Rml::Input::KI_RIGHT;
		case sf::Keyboard::Key::Up:        return Rml::Input::KI_UP;
		case sf::Keyboard::Key::Down:      return Rml::Input::KI_DOWN;
		case sf::Keyboard::Key::Add:       return Rml::Input::KI_ADD;
		case sf::Keyboard::Backspace:      return Rml::Input::KI_BACK;
		case sf::Keyboard::Key::Delete:    return Rml::Input::KI_DELETE;
		case sf::Keyboard::Key::Divide:    return Rml::Input::KI_DIVIDE;
		case sf::Keyboard::Key::End:       return Rml::Input::KI_END;
		case sf::Keyboard::Key::Escape:    return Rml::Input::KI_ESCAPE;
		case sf::Keyboard::Key::F1:        return Rml::Input::KI_F1;
		case sf::Keyboard::Key::F2:        return Rml::Input::KI_F2;
		case sf::Keyboard::Key::F3:        return Rml::Input::KI_F3;
		case sf::Keyboard::Key::F4:        return Rml::Input::KI_F4;
		case sf::Keyboard::Key::F5:        return Rml::Input::KI_F5;
		case sf::Keyboard::Key::F6:        return Rml::Input::KI_F6;
		case sf::Keyboard::Key::F7:        return Rml::Input::KI_F7;
		case sf::Keyboard::Key::F8:        return Rml::Input::KI_F8;
		case sf::Keyboard::Key::F9:        return Rml::Input::KI_F9;
		case sf::Keyboard::Key::F10:       return Rml::Input::KI_F10;
		case sf::Keyboard::Key::F11:       return Rml::Input::KI_F11;
		case sf::Keyboard::Key::F12:       return Rml::Input::KI_F12;
		case sf::Keyboard::Key::F13:       return Rml::Input::KI_F13;
		case sf::Keyboard::Key::F14:       return Rml::Input::KI_F14;
		case sf::Keyboard::Key::F15:       return Rml::Input::KI_F15;
		case sf::Keyboard::Key::Home:      return Rml::Input::KI_HOME;
		case sf::Keyboard::Key::Insert:    return Rml::Input::KI_INSERT;
		case sf::Keyboard::Key::LControl:  return Rml::Input::KI_LCONTROL;
		case sf::Keyboard::Key::LShift:    return Rml::Input::KI_LSHIFT;
		case sf::Keyboard::Key::Multiply:  return Rml::Input::KI_MULTIPLY;
		case sf::Keyboard::Key::Pause:     return Rml::Input::KI_PAUSE;
		case sf::Keyboard::Key::RControl:  return Rml::Input::KI_RCONTROL;
		case sf::Keyboard::Enter:          return Rml::Input::KI_RETURN;
		case sf::Keyboard::Key::RShift:    return Rml::Input::KI_RSHIFT;
		case sf::Keyboard::Key::Space:     return Rml::Input::KI_SPACE;
		case sf::Keyboard::Key::Subtract:  return Rml::Input::KI_SUBTRACT;
		case sf::Keyboard::Key::Tab:       return Rml::Input::KI_TAB;
		default: break;
		}

		return Rml::Input::KI_UNKNOWN;

	}
	int getKeyModifierState() {
		int modifiers = 0;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
			modifiers |= Rml::Input::KM_SHIFT;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
			modifiers |= Rml::Input::KM_CTRL;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt))
			modifiers |= Rml::Input::KM_ALT;

		return modifiers;
	}

	// Rml::FileInterface requiers us to return an OS file handle but 
	// in our case VirtualFileSystem is one big container that contains all 
	// our files so returning real file handles are meaningless so
	// we introduce a virtualFileHandles counter that get incremented at 
	// everytime RmlUi request for a file
	//
	static uint64_t virtualFileHandles = 1;

	struct RmlFile {
		io::stream::MemoryInputStream* memIn;
		Rml::String fTag;
	};
	FileInterface_dms::FileInterface_dms(io::VirtualFileSystem& vfs) : vfs(vfs) {

	}

    FileInterface_dms::~FileInterface_dms() {
	
	}
	Rml::FileHandle FileInterface_dms::Open(const Rml::String& fTag) {
		if (FileInterface_dms::vfs.findFileTag(fTag)) {

			void* buffer = nullptr; // <- Input 
			size_t size = 0;
			vfs.getFile(fTag.c_str(), buffer, size);

			RmlFile* file = new RmlFile();
			file->fTag = fTag;
			file->memIn = new io::stream::MemoryInputStream(buffer, size);
			return Rml::FileHandle(file);
		}
		else {
			return 0;// nullptr/file tag not found
		}
	}
	void FileInterface_dms::Close(Rml::FileHandle hFile) {
		RmlFile* file = (RmlFile*)hFile;
		if (file != nullptr) {
			file->memIn->freeMemoryBlock();
			delete file->memIn;
			file->memIn = nullptr;
			delete file;
			file = nullptr;
		}
	}
	size_t FileInterface_dms::Read(void* buffer, size_t size, Rml::FileHandle hFile) {
		RmlFile* file = (RmlFile*)hFile;
		if (file != nullptr) {
			return file->memIn->read(buffer, size);
		}
		else {
			return 0;
		}
	}
	bool FileInterface_dms::Seek(Rml::FileHandle hFile, long offset, int origin) {
		RmlFile* file = (RmlFile*)hFile;
		if (file != nullptr) {
			return (bool)file->memIn->seek(offset);
		}
		return false;
	}
	size_t FileInterface_dms::Tell(Rml::FileHandle hFile) {
		RmlFile* file = (RmlFile*)hFile;
		if (file != nullptr) {
			return (bool)file->memIn->tell();
		}
		return false;
	}
	size_t FileInterface_dms::Length(Rml::FileHandle hFile) {
		RmlFile* file = (RmlFile*)hFile;
		if (file != nullptr) {
			return file->memIn->getSize();
		}
		return false;
	}

	bool FileInterface_dms::LoadFile(const Rml::String& fTag, Rml::String& out_data) {
		if (vfs.findFileTag(fTag)) {
			void* buffer = nullptr;
			size_t size = 0;
			vfs.getFile("", buffer, size);
			out_data.append((char*)buffer);
			free(buffer);
			return true;
		}
		return false;
	}
}