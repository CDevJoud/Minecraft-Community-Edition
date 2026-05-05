#pragma once
#include "Renderer.hpp"
#include "..\IO\VirtualFileSystem.hpp"
#include "RenderStates.hpp"
#include "Renderable.hpp"

namespace mce::gfx {

	class Void final: private Renderer {
	public:
		Void(uint16_t viewId, RenderFactory& factory, RenderContext::API api);
		~Void();
		void init(io::VirtualFileSystem& vfs);
		
		void _render();

		void render(const eastl::shared_ptr<VertexBuffer>&vBuffer, RenderStates& states);

		void render(const Renderable& renderable);
	private:
		bgfx::VertexLayout vLayout;
		eastl::shared_ptr<ShaderProgram> sp = nullptr;
		float proj[16];
		int width, height;
	};
}
