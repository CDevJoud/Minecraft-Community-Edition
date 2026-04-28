#include "VertexBuffer.hpp"

namespace mce::gfx {
	VertexBuffer::VertexBuffer(const VertexArray& vArray, flags::Buffer bFlags, bool& success) {
		const bgfx::Memory* mem = bgfx::copy(vArray.vertices.data(), vArray.vertices.size() * vArray.vertexSizeInMem);
		VertexBuffer::vbh = bgfx::createVertexBuffer(mem, vArray.vLayout, VertexBuffer::bFlag.bFlag);
		success = bgfx::isValid(VertexBuffer::vbh);

		count = vArray.vertices.size();
	}
	VertexBuffer::~VertexBuffer() {
		bgfx::destroy(VertexBuffer::vbh);
		VertexBuffer::vbh = BGFX_INVALID_HANDLE;
	}
	bgfx::VertexBufferHandle VertexBuffer::getNativeHandle() const {
		return VertexBuffer::vbh;
	}

	void VertexBuffer::bind(const VertexBuffer& vBuffer, uint8_t stream) {
		bgfx::setVertexBuffer(stream, vBuffer.vbh);
	}

	void VertexBuffer::bind(const eastl::shared_ptr<VertexBuffer>& vBuffer, uint8_t stream) {
		if (vBuffer != nullptr) {
			bgfx::setVertexBuffer(stream, vBuffer.get()->vbh);
		}
	}
	
}
