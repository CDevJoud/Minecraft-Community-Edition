#pragma once
#include <bgfx/bgfx.h>
#include <EASTL/shared_ptr.h>

namespace mce::gfx {
	namespace flags {
		struct Buffer;
	}
	class RenderFactory;

	class IndexBuffer {
		friend class RenderFactory;
		template<typename... Args>
		static eastl::shared_ptr<IndexBuffer> createInstance(Args&&... args) {
			struct EnableMakeShared : public IndexBuffer {
				EnableMakeShared(Args&&... a) : IndexBuffer(eastl::forward<Args>(a)...) {}
			};

			return eastl::make_shared<EnableMakeShared>(eastl::forward<Args>(args)...);
		}

		IndexBuffer(const bgfx::Memory* mem, flags::Buffer bFlags, bool& success);
		~IndexBuffer();

	public:
		bgfx::IndexBufferHandle getNativeHandle() const;

		static void bind(const IndexBuffer& iBuffer, uint8_t stream = 0);

		static void bind(const eastl::shared_ptr<IndexBuffer>& iBuffer, uint8_t stream = 0);

	private:
		bgfx::IndexBufferHandle ibh;

	};
}
