#include "FileInputStream.hpp"

#include <filesystem>

namespace mce {
	FileInputStream::FileInputStream(const eastl::string_view file) {
		open(file);
	}

	bool FileInputStream::isOpen() const {
		return stream.is_open();
	}

	bool FileInputStream::eof() const {
		return stream.eof();
	}

	void FileInputStream::close() {
		stream.close();
	}

	void FileInputStream::open(const eastl::string_view file) {
		stream.open(file.data());
		fileName = file.data();
	}

	size_t FileInputStream::read(void* buffer, const size_t size) {
		stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(size));
		return stream.gcount();
	}

	size_t FileInputStream::seek(const size_t position) {
		stream.seekg(static_cast<std::streamsize>(position));
		return position;
	}

	size_t FileInputStream::tell() {
		return stream.tellg();
	}

	size_t FileInputStream::getSize() {
		return std::filesystem::file_size(fileName);
	}
}
