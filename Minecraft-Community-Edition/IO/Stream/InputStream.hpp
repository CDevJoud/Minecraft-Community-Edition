#pragma once

#include <cstdint>

namespace mce {
	class InputStream {
	public:
		virtual ~InputStream() = 0;

		virtual size_t read(void* buffer, size_t size) = 0;
		virtual size_t seek(size_t position) = 0;
		virtual size_t tell() = 0;
		virtual size_t getSize() = 0;
	};
}