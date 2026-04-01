#include "Common/Platform.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#include <Windows.h>
#include "FileInputStream.hpp"
#ifdef INVALID_FILE_SIZE
#undef INVALID_FILE_SIZE
#endif

namespace mce {

	FileInputStream::FileInputStream() {
		FileInputStream::hFile = INVALID_HANDLE_VALUE;
	}

	FileInputStream::~FileInputStream() {
		FileInputStream::close();
	}

	bool FileInputStream::open(const std::string& path) {
		FileInputStream::hFile = CreateFileA(
			path.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		return FileInputStream::hFile != INVALID_HANDLE_VALUE;
	}

	void FileInputStream::close() {
		if (FileInputStream::hFile != INVALID_HANDLE_VALUE) {
			CloseHandle((HANDLE)FileInputStream::hFile);
			FileInputStream::hFile = INVALID_HANDLE_VALUE;
		}
	}

    bool FileInputStream::isOpen() const {
		return FileInputStream::hFile != INVALID_HANDLE_VALUE;
	}

	size_t FileInputStream::read(void* buffer, size_t size) {
		DWORD bytesRead = 0;

		if (!ReadFile((HANDLE)FileInputStream::hFile, buffer, (DWORD)size, &bytesRead, nullptr))
			return 0;

		return bytesRead;
	}

	size_t FileInputStream::seek(size_t position) {
		LARGE_INTEGER offset{};
		offset.QuadPart = position;

		SetFilePointerEx((HANDLE)FileInputStream::hFile, offset, nullptr, FILE_BEGIN);
		return position;
	}

	size_t FileInputStream::tell() {
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER pos;

		SetFilePointerEx((HANDLE)FileInputStream::hFile, zero, &pos, FILE_CURRENT);

		return (size_t)pos.QuadPart;
	}

	size_t FileInputStream::getSize() {
		LARGE_INTEGER size;
		
		GetFileSizeEx((HANDLE)FileInputStream::hFile, &size);

		return (size_t)size.QuadPart;
	}

}

#endif

#if defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
#pragma warning(disable:4996)
#include "FileInputStream.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include "FileOutputStream.hpp"

namespace mce {

	FileInputStream::FileInputStream() : 
		file(nullptr) {}

	FileInputStream::~FileInputStream() {
		FileInputStream::close();
	}

	bool FileInputStream::open(const std::string& fileName) {
		if (FileInputStream::file)
			std::fclose(FileInputStream::file);

		FileInputStream::file = std::fopen(fileName.c_str(), "rb");

		return FileInputStream::file != NULL;
	}

	void FileInputStream::close() {
		if (FileInputStream::file != nullptr) {
			std::fclose(FileInputStream::file);
			FileInputStream::file = nullptr;
		}
	}

	bool FileInputStream::isOpen() const {
		return FileInputStream::file != nullptr;
	}

	size_t FileInputStream::read(void* buffer, size_t size) {
		if (FileInputStream::file)
			return std::fread(buffer, 1, size, FileInputStream::file);
		else
			return 0;
	}

	size_t FileInputStream::seek(size_t position) {
		if (FileInputStream::file) {
			if (std::fseek(FileInputStream::file, static_cast<long>(position), SEEK_SET))
				return FileInputStream::INVALID_FILE_POSITION;

			return tell();
		}
		else {
			return FileInputStream::INVALID_FILE_POSITION;
		}
	}

	size_t FileInputStream::tell() {
		if (FileInputStream::file)
			return std::ftell(FileInputStream::file);
		else
			return FileInputStream::INVALID_FILE_POSITION;
	}

	size_t FileInputStream::getSize() {
		if (FileInputStream::file) {
			size_t position = FileInputStream::tell();
			std::fseek(FileInputStream::file, 0, SEEK_END);
			size_t size = FileInputStream::tell();
			FileInputStream::seek(position);
			return size;
		}
		else {
			return FileInputStream::INVALID_FILE_SIZE;
		}
	}
}

#endif
