#include "Void.hpp"
#include "..\Platform.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable : 4996)
#endif
#include <bgfx/bgfx.h>

using mce::io::VirtualFileSystem;

#define LOG_ERROR(...) qBus.post(event::Log(event::Log::ERROR, std::format(__VA_ARGS__)))

namespace mce::gfx {
	Void::Void(uint16_t viewId, RenderFactory& factory, RenderContext::API api) : Renderer(viewId, factory, api) {

	}
	Void::~Void() {
	
	}
	std::string getRenderName(RenderContext::API api) {
		switch (api) {
		case mce::gfx::RenderContext::API::Noop:
			break;
		case mce::gfx::RenderContext::API::Agc:
			break;
		case mce::gfx::RenderContext::API::Direct3D11:
			return "d3d11_";
		case mce::gfx::RenderContext::API::Direct3D12:
			return "d3d12_";
		case mce::gfx::RenderContext::API::Gnm:
			break;
		case mce::gfx::RenderContext::API::Metal:
			return "metal_";
		case mce::gfx::RenderContext::API::Nvn:
			break;
		case mce::gfx::RenderContext::API::OpenGLES:
			break;
		case mce::gfx::RenderContext::API::OpenGL:
			return "gl_";
		case mce::gfx::RenderContext::API::Vulkan:
			return "vulkan_";
		case mce::gfx::RenderContext::API::WebGPU:
			break;
		case mce::gfx::RenderContext::API::Count:
			break;
		default:
			break;
		}
	}
	void Void::init(VirtualFileSystem& vfs) {
		VertexArray vArray;
		vArray.append(Vertex(sf::Vector3f(1.0f, 1.0f, 0.0f), sf::Color::Red, sf::Vector2f(1.0f, 1.0f)));
		vLayout = Vertex::layout();
		vArray.setVertexLayout(vLayout, sizeof(Vertex));

		bool success = false;
		flags::Buffer vbFlag;

		vbFlag.addFlag(flags::Buffer::None);

		RenderFactory& factory = Void::getFactory();

		Void::vb = factory.createVertexBuffer(vArray, vbFlag, "Void:VertexBuffer");
		if (Void::vb == nullptr) {
			//Error
			return;
		}
		
		std::string fName = "assets.shaders.main.fs." + getRenderName(Void::getRendererAPI()) + PLATFORM_NAME;
		eastl::vector<uint8_t> fsbytes;
		vfs.getFile(fName.c_str(), fsbytes);

		fName = "assets.shaders.main.vs." + getRenderName(Void::getRendererAPI()) + PLATFORM_NAME;
		eastl::vector<uint8_t> vsbytes;
		vfs.getFile(fName.c_str(), vsbytes);

		Void::sp = factory.createShaderProgram(eastl::make_pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>(vsbytes, fsbytes));
		if (Void::sp == nullptr) {
			//Error
			//LOG_ERROR("could not create shader program");
			
			return;
		}

	}
	void Void::render() {
		Renderer::addFlag(Void::Clear::Color);
		Renderer::addFlag(Void::Clear::Depth);
		Renderer::setClearColor(0x00, 0xFF, 0x00);

		VertexBuffer::bind(vb);

		Renderer::touch();
	}
}
