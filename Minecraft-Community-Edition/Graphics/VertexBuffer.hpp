#pragma once
#include "VertexArray.hpp"
#include "Flags.hpp"
#include <EASTL/shared_ptr.h>

namespace mce::gfx {
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

		VertexBuffer(const VertexArray& vArray, flags::Buffer bFlags, bool& success);
		~VertexBuffer();

	public:

		size_t getVertexCount() const;
		bgfx::VertexBufferHandle getNativeHandle() const;

		flags::Buffer bFlag; // buffer flag

		static void bind(const VertexBuffer& vBuffer, uint8_t stream = 0);

		static void bind(const eastl::shared_ptr<VertexBuffer>& vBuffer, uint8_t stream = 0);
	private:
		bgfx::VertexBufferHandle vbh;
		size_t count;
	};

}
