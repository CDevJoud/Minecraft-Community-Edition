#include "..\..\Platform.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "FileOutputStream.hpp"

namespace mce::io::stream {
	bool FileOutputStream::open(const std::string& file) {
		FileOutputStream::hFile = CreateFile(file.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		return (FileOutputStream::hFile != INVALID_HANDLE_VALUE);
	}

	bool FileOutputStream::isOpen() const {
		return (FileOutputStream::hFile != INVALID_HANDLE_VALUE);
	}
	
	void FileOutputStream::close() {
		if (FileOutputStream::hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(FileOutputStream::hFile);
		}
	}

	size_t FileOutputStream::write(const void* buffer, size_t size) {
		DWORD w;
		WriteFile(FileOutputStream::hFile, buffer, size, &w, nullptr);
		return static_cast<size_t>(w);
	}

	size_t FileOutputStream::seek(size_t position) {
		LARGE_INTEGER offset{};
		offset.QuadPart = position;
		if (SetFilePointerEx(FileOutputStream::hFile, offset, nullptr, FILE_BEGIN) < 0) {
			return FileOutputStream::INVALID_FILE_POSITION;
		}
		return position;
	}

	size_t FileOutputStream::tell() {
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER pos;

		if (SetFilePointerEx((HANDLE)FileOutputStream::hFile, zero, &pos, FILE_CURRENT) < 0) {
			return FileOutputStream::INVALID_FILE_POSITION;
		}

		return (size_t)pos.QuadPart;
	}

	size_t FileOutputStream::getSize() {
		LARGE_INTEGER size{};
		if (GetFileSizeEx(FileOutputStream::hFile, &size) > 0) {
			return FileOutputStream::INVALID_FILE_SIZE;
		}
		return (size_t)size.QuadPart;
	}
}
#endif

#if defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
#pragma warning(disable:4996)

#include <fcntl.h>
#include <sys/stat.h>
#include "FileOutputStream.hpp"

namespace mce {

	FileOutputStream::FileOutputStream() :
		file(nullptr) {}

	FileOutputStream::~FileOutputStream() {
		FileOutputStream::close();
	}

	bool FileOutputStream::open(const std::string& fileName) {
		if (FileOutputStream::file)
			std::fclose(FileOutputStream::file);

		FileOutputStream::file = std::fopen(fileName.c_str(), "wb");

		return FileOutputStream::file != NULL;
	}

	void FileOutputStream::close() {
		if (FileOutputStream::file != nullptr) {
			std::fclose(FileOutputStream::file);
			FileOutputStream::file = nullptr;
		}
	}

	bool FileOutputStream::isOpen() const {
		return FileOutputStream::file != nullptr;
	}

	size_t FileOutputStream::write(const void* buffer, size_t size) {
		if (FileOutputStream::file)
			return std::fwrite(buffer, 1, size, FileOutputStream::file);
		else
			return 0;
	}

	size_t FileOutputStream::seek(size_t position) {
		if (FileOutputStream::file) {
			if (std::fseek(FileOutputStream::file, static_cast<long>(position), SEEK_SET))
				return FileOutputStream::INVALID_FILE_POSITION;

			return tell();
		}
		else {
			return FileOutputStream::INVALID_FILE_POSITION;
		}
	}

	size_t FileOutputStream::tell() {
		if (FileOutputStream::file)
			return std::ftell(FileOutputStream::file);
		else
			return FileOutputStream::INVALID_FILE_POSITION;
	}

	size_t FileOutputStream::getSize() {
		if (FileOutputStream::file) {
			size_t position = FileOutputStream::tell();
			std::fseek(FileOutputStream::file, 0, SEEK_END);
			size_t size = FileOutputStream::tell();
			FileOutputStream::seek(position);
			return size;
		}
		else {
			return FileOutputStream::INVALID_FILE_SIZE;
		}
	}
}

#endif

