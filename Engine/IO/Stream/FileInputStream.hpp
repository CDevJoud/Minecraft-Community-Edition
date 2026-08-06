#pragma once

#include "..\..\Platform.hpp"
#include <fstream>

#include "InputStream.hpp"

#include <EASTL/string_view.h>

#ifdef MCE_PLATFORM_WINDOWS
#ifdef INVALID_FILE_SIZE
#undef INVALID_FILE_SIZE
#endif
#endif

namespace dms::io::stream {
	class FileInputStream final : public InputStream {
	public:
		FileInputStream();
		~FileInputStream();
		/**
		 * @brief Constructs the stream and opens a file
		 * @param file The path (absolute or relative) to the file
		 */
		//explicit FileInputStream(eastl::string_view file);

		/**
		 * @brief Opens a file to read from
		 * @param file The path (absolute or relative) to the file
		 */
		bool open(const std::string& file);

		/**
		 * @return True if the file was successfully opened
		 */
		bool isOpen() const;

		/**
		 * @brief Closes the file
		 */
		void close();

		/**
		 * @brief Reads from the input stream
		 * @param buffer The buffer to read the bytes into
		 * @param size The amount of bytes to read
		 * @return The amount of bytes written
		 */
		size_t read(void* buffer, size_t size) override;

		/**
		 * @brief Changes the current position the stream is reading
		 * @param position The position to seek the input stream to
		 * @return The position the input stream is at now
		 */
		size_t seek(size_t position) override;

		/**
		 * @return The current position the stream is at
		 */
		size_t tell() override;

		/**
		 * @return The size of the stream in bytes
		 */
		size_t getSize() override;

		static inline constexpr size_t FILE_NOT_FOUND = 0;
		static inline constexpr size_t INVALID_FILE_SIZE = ~(0);
		static inline constexpr size_t INVALID_FILE_POSITION = ~(0);
	private:
#ifdef MCE_PLATFORM_WINDOWS
		void* __ptr64 hFile = nullptr;
#elif defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
		std::FILE* file;
#endif
	};
}