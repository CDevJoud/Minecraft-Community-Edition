#pragma once

#include <cstdint>

namespace mce {
	class OutputStream {
	public:
		virtual ~OutputStream() = 0;

		/**
		 * @brief Writes all pending operations to the file
		 */
		virtual void flush() = 0;

		virtual size_t write(void* buffer, size_t size) = 0;
		virtual size_t seek(size_t position) = 0;
		virtual size_t tell() = 0;
		virtual size_t getSize() = 0;
	};
}