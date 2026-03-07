#pragma once

#include <fstream>

#include "InputStream.hpp"

#include <EASTL/string_view.h>

namespace mce {
	class FileInputStream final : public InputStream {
	public:
		FileInputStream() = default;

		/**
		 * @brief Constructs the stream and opens a file
		 * @param file The path (absolute or relative) to the file
		 */
		explicit FileInputStream(eastl::string_view file);

		/**
		 * @brief Opens a file to read from
		 * @param file The path (absolute or relative) to the file
		 */
		void open(eastl::string_view file);

		/**
		 * @return True if the file was successfully opened
		 */
		bool isOpen() const;

		/**
		 * @brief Checks if the stream is at the end of the file
		 * @return True if the file stream has the end-of-file flag (EOF).
		 */
		bool eof() const;

		/**
		 * @brief Closes the file
		 */
		void close();

		size_t read(void* buffer, size_t size) override;
		size_t seek(size_t position) override;
		size_t tell() override;
		size_t getSize() override;
	private:
		std::ifstream stream;
		std::string fileName;
	};
}