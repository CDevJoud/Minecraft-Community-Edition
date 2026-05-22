#include "IndexBuffer.hpp"
#include "Flags.hpp"

namespace mce::gfx{
	IndexBuffer::IndexBuffer(const bgfx::Memory* mem, flags::Buffer bFlags, bool& success) {
		IndexBuffer::ibh = bgfx::createIndexBuffer(mem, bFlags.bFlag);
		success = bgfx::isValid(IndexBuffer::ibh);
	}
	IndexBuffer::~IndexBuffer() {
		if (bgfx::isValid(IndexBuffer::ibh)) {
			bgfx::destroy(IndexBuffer::ibh);
			IndexBuffer::ibh = BGFX_INVALID_HANDLE;
		}
	}
	bgfx::IndexBufferHandle IndexBuffer::getNativeHandle() const {
		return IndexBuffer::ibh;
	}
}
