#pragma once
#include "Vertex.hpp"
#include <EASTL/vector.h>

namespace mce::gfx {
	template<typename T>
	class VertexArray_t {
	public:
		VertexArray_t() : vertexSizeInMem(sizeof(Vertex)) {

		}

		void setVertexLayout(const bgfx::VertexLayout& layout, size_t vertexSizeInMem = sizeof(T)) {
			VertexArray_t::vLayout = layout;
			VertexArray_t::vertexSizeInMem = vertexSizeInMem;
		}

		size_t getVertexCount() const {
			return VertexArray_t::vertices.size();
		}

		T& operator[](size_t index) {
			return VertexArray_t::vertices[index];
		}

		const T& operator[](size_t index) const {
			return VertexArray_t::vertices[index];
		}

		void clear() {
			VertexArray_t::vertices.clear();
		}

		void resize(size_t count) {
			VertexArray_t::vertices.resize(count);
		}

		void append(const T& vertex) {
			VertexArray_t::vertices.push_back(vertex);
		}

	private:
		friend class VertexBuffer;
		friend class RenderFactory;
		bgfx::VertexLayout vLayout;
		size_t vertexSizeInMem;
		eastl::vector<T> vertices;
	};
	using VertexArray = VertexArray_t<Vertex>;
}
