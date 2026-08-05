#pragma once
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/FileInterface.h>

#include <Core/WindowBase.hpp>
#include <Core/Event.hpp>
#include <Core/Clock.hpp>
#include <Core/QEventBus.hpp>

#include <IO/VirtualFileSystem.hpp>

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

	class FileInterface_dms : public Rml::FileInterface {
	public:
		FileInterface_dms(io::VirtualFileSystem& vfs);
		virtual ~FileInterface_dms();

		/// Opens a file.
		/// @param path The path to the file to open.
		/// @return A valid file handle, or nullptr on failure
		virtual Rml::FileHandle Open(const Rml::String& path) override;
		/// Closes a previously opened file.
		/// @param file The file handle previously opened through Open().
		virtual void Close(Rml::FileHandle file) override;

		/// Reads data from a previously opened file.
		/// @param buffer The buffer to be read into.
		/// @param size The number of bytes to read into the buffer.
		/// @param file The handle of the file.
		/// @return The total number of bytes read into the buffer.
		virtual size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;
		/// Seeks to a point in a previously opened file.
		/// @param file The handle of the file to seek.
		/// @param offset The number of bytes to seek.
		/// @param origin One of either SEEK_SET (seek from the beginning of the file), SEEK_END (seek from the end of the file) or SEEK_CUR (seek from
		/// the current file position).
		/// @return True if the operation completed successfully, false otherwise.
		virtual bool Seek(Rml::FileHandle file, long offset, int origin) override;
		/// Returns the current position of the file pointer.
		/// @param file The handle of the file to be queried.
		/// @return The number of bytes from the origin of the file.
		virtual size_t Tell(Rml::FileHandle file) override;

		/// Returns the length of the file.
		/// The default implementation uses Seek & Tell.
		/// @param file The handle of the file to be queried.
		/// @return The length of the file in bytes.
		virtual size_t Length(Rml::FileHandle file);

		/// Load and return a file.
		/// @param path The path to the file to load.
		/// @param out_data The string contents of the file.
		/// @return True on success.
		virtual bool LoadFile(const Rml::String& path, Rml::String& out_data);
	private:
		io::VirtualFileSystem& vfs;
	};

	bool inputEventHandler(Rml::Context* ctx, sf::WindowHandle hWnd, sf::Event& event);

	Rml::Input::KeyIdentifier convertKey(sf::Keyboard::Key key);

	int getKeyModifierState();
}
