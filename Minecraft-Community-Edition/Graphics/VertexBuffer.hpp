#pragma once
#include "VertexArray.hpp"
#include "Flags.hpp"
#include <EASTL/shared_ptr.h>

namespace mce::gfx {
	class VertexBuffer {
	public:
		VertexBuffer(const VertexArray& vArray, flags::Buffer bFlags, bool& success);
		~VertexBuffer();

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
