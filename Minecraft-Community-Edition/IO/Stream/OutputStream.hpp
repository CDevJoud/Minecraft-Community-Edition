#pragma once

#include <cstdint>

namespace mce {
	class OutputStream {
	public:
		virtual ~OutputStream() = 0;

		/**
		 * @param buffer The data to write
		 * @param size How many bytes to write
		 * @return How many bytes were written
		 */
		virtual size_t write(const void* buffer, size_t size) = 0;

		/**
		 * @brief Changes the current position the stream is at
		 * @param position The position to seek the output stream to
		 * @return The position the output stream is at now
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