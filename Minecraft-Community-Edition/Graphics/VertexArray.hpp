#pragma once
#include "Vertex.hpp"
#include <EASTL/vector.h>

namespace mce::gfx {
	class VertexArray {
	public:
		VertexArray();

		void setVertexLayout(bgfx::VertexLayout& layout, size_t vertexSizeInMem = sizeof(Vertex));

		size_t getVertexCount() const;

		Vertex& operator[](size_t index);

		const Vertex& operator[](size_t index) const;

		void clear();

		void resize(size_t count);

		void append(const Vertex& vertex);

	private:
		friend class VertexBuffer;
		friend class RenderFactory;
		bgfx::VertexLayout vLayout;
		size_t vertexSizeInMem;
		eastl::vector<Vertex> vertices;
	};
}
