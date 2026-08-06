#include "MemoryInputStream.hpp"
#include <algorithm>

#include <cstring>


namespace dms::io::stream {
	MemoryInputStream::MemoryInputStream(const void* data, size_t size) : 
		data(static_cast<const char*>(data)), size(size) {
		
	}
	size_t MemoryInputStream::read(void* buffer, size_t size) {
		if (!MemoryInputStream::data)
			return ~(0);

		const size_t count = std::min(size, MemoryInputStream::size - MemoryInputStream::offset);
		if (count > 0) {
			std::memcpy(buffer, MemoryInputStream::data + MemoryInputStream::offset, static_cast<size_t>(count));
			MemoryInputStream::offset += count;
		}

		return count;
	}
    size_t MemoryInputStream::readString(char*& buffer, size_t& size) {
        
		if (!MemoryInputStream::data)
			return ~(0);

		size = strlen(MemoryInputStream::data + MemoryInputStream::offset);
		buffer = (char*&)MemoryInputStream::data + MemoryInputStream::offset;
		MemoryInputStream::offset += size + 1;
		return size + 1; // the zero terminate byte does count so we add one
    }
    size_t MemoryInputStream::seek(size_t position) {
		if (!MemoryInputStream::data)
			return ~(0);

		MemoryInputStream::offset = position < MemoryInputStream::size ? position : MemoryInputStream::size;
		return MemoryInputStream::offset;
	}
	size_t MemoryInputStream::tell() {
		if (!MemoryInputStream::data)
			return ~(0);

		return MemoryInputStream::offset;
	}
	size_t MemoryInputStream::getSize() {
		if (!MemoryInputStream::data)
			return ~(0);

		return MemoryInputStream::size;
	}
    void MemoryInputStream::freeMemoryBlock() {
		delete MemoryInputStream::data;
		MemoryInputStream::offset = 0;
		MemoryInputStream::size = 0;
	}
}