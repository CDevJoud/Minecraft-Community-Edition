#pragma once
#include "VertexArray.hpp"
#include "Flags.hpp"
#include <EASTL/shared_ptr.h>

namespace dms::gfx {
	class RenderFactory;

	class VertexBuffer {
		friend class RenderFactory;

		template <typename... Args>
		static eastl::shared_ptr<VertexBuffer> createInstance(Args&&... args) {
			struct EnableMakeShared : public VertexBuffer {
				EnableMakeShared(Args&&... a) : VertexBuffer(eastl::forward<Args>(a)...) {}
			};

			return eastl::make_shared<EnableMakeShared>(eastl::forward<Args>(args)...);
		}
		VertexBuffer(const VertexArray& vArray, flags::Buffer bFlags, bool& success) {
			const bgfx::Memory* mem = bgfx::copy(vArray.vertices.data(), vArray.vertices.size() * vArray.vertexSizeInMem);
			VertexBuffer::vbh = bgfx::createVertexBuffer(mem, vArray.vLayout, VertexBuffer::bFlag.bFlag);
			success = bgfx::isValid(VertexBuffer::vbh);
			
			count = vArray.vertices.size();
		}
		VertexBuffer(const bgfx::Memory* mem, bgfx::VertexLayout vLayout, flags::Buffer bFlag, bool& success) {
			VertexBuffer::vbh = bgfx::createVertexBuffer(mem, vLayout, bFlag.bFlag);
			success = bgfx::isValid(VertexBuffer::vbh);

			count = mem->size;
		}
		

	public:
		~VertexBuffer() {
			bgfx::destroy(VertexBuffer::vbh);
			VertexBuffer::vbh = BGFX_INVALID_HANDLE;
		}
		size_t getVertexCount() const;
		bgfx::VertexBufferHandle getNativeHandle() const {
			return VertexBuffer::vbh;
		}

		flags::Buffer bFlag; // buffer flag

		static void bind(const VertexBuffer& vBuffer, uint8_t stream = 0) {
			bgfx::setVertexBuffer(stream, vBuffer.vbh);
		}

		static void bind(const eastl::shared_ptr<VertexBuffer>& vBuffer, uint8_t stream = 0) {
			if (vBuffer != nullptr) {
				bgfx::setVertexBuffer(stream, vBuffer.get()->vbh);
			}
		}
	private:
		bgfx::VertexBufferHandle vbh;
		size_t count;
	};
}
