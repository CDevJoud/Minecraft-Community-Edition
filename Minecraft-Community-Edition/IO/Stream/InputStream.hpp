#pragma once

#include <cstdint>

namespace mce::io::stream {
	class InputStream {
	public:
		virtual ~InputStream() = 0;

		/**
		 * @brief Reads from the input stream
		 * @param buffer The buffer to read the bytes into
		 * @param size The amount of bytes to read
		 * @return The amount of bytes written
		 */
		virtual size_t read(void* buffer, size_t size) = 0;

		/**
		 * @brief Changes the current position the stream is reading
		 * @param position The position to seek the input stream to
		 * @return The position the input stream is at now
		 */
		virtual size_t seek(size_t position) = 0;

		/**
		 * @return The current position the stream is at
		 */
		virtual size_t tell() = 0;

		/**
		 * @return The size of the stream in bytes
		 */
		virtual size_t getSize() = 0;
	};
}