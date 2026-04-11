#pragma once
#include "Renderer.hpp"
#include "..\IO\VirtualFileSystem.hpp"

namespace mce::gfx {
	class Void final: private Renderer {
	public:
		Void(uint16_t viewId, RenderFactory& factory, RenderContext::API api);
		~Void();
		void init(io::VirtualFileSystem& vfs);
		
		void render();
	private:
		bgfx::VertexLayout vLayout;
		eastl::shared_ptr<VertexBuffer> vb = nullptr;
		eastl::shared_ptr<ShaderProgram> sp = nullptr;
	};
}
