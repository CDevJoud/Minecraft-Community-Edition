#include "Void.hpp"
#include "..\Platform.hpp"
#include "Core/ddspp.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable : 4996)
#endif
#include "libs/bgfx/bgfx.h"
#include "libs/bx/math.h"

using dms::io::VirtualFileSystem;

#define LOG_ERROR(...) qBus.post(event::Log(event::Log::ERROR, std::format(__VA_ARGS__)))

namespace dms::gfx {
	Void::Void(uint16_t viewId, RenderFactory& factory, RenderContext::API api) : Renderer(viewId, factory, api) {
		/*ddspp::Descriptor desc;
		ddspp::Result res = ddspp::decode_header("", desc);*/
		
	}
	Void::~Void() {
	
	}
	std::string getRenderName(RenderContext::API api) {
		switch (api) {
		case dms::gfx::RenderContext::API::Noop:
			break;
		case dms::gfx::RenderContext::API::Agc:
			break;
		case dms::gfx::RenderContext::API::Direct3D11:
			return "d3d11_";
		case dms::gfx::RenderContext::API::Direct3D12:
			return "d3d12_";
		case dms::gfx::RenderContext::API::Gnm:
			break;
		case dms::gfx::RenderContext::API::Metal:
			return "metal_";
		case dms::gfx::RenderContext::API::Nvn:
			break;
		case dms::gfx::RenderContext::API::OpenGLES:
			break;
		case dms::gfx::RenderContext::API::OpenGL:
			return "gl_";
		case dms::gfx::RenderContext::API::Vulkan:
			return "vulkan_";
		case dms::gfx::RenderContext::API::WebGPU:
			break;
		case dms::gfx::RenderContext::API::Count:
			break;
		default:
			break;
		}
	}
	void Void::init(VirtualFileSystem& vfs) {

		RenderFactory& factory = Renderer::getFactory();

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

		bx::mtxProj(
			proj,
			90.0f,
			getViewSpace().width / getViewSpace().height,
			0.1f, 1000.0f,
			bgfx::getCaps()->homogeneousDepth
		);

		//VertexArray vArray;
		//vArray.append(Vertex(sf::Vector3f(-0.5f, -0.5f, 0.0f), sf::Color::Transparent, sf::Vector2f(0.0f, 1.0f)));
		//vArray.append(Vertex(sf::Vector3f( 0.5f, -0.5f, 0.0f), sf::Color::Transparent, sf::Vector2f(1.0f, 1.0f)));
		//vArray.append(Vertex(sf::Vector3f( 0.0f,  0.5f, 0.0f), sf::Color::Transparent, sf::Vector2f(0.5f, 0.0f)));
		//vLayout = Vertex::layout();
		//vArray.setVertexLayout(vLayout, sizeof(Vertex));

		//bool success = false;
		//flags::Buffer vbFlag;

		//vbFlag.addFlag(flags::Buffer::None);

		//RenderFactory& factory = Void::getFactory();

		//Void::vb = factory.createVertexBuffer(vArray, vbFlag, "Void:VertexBuffer");
		//if (Void::vb == nullptr) {
		//	//Error
		//	return;
		//}
		//
		//std::string fName = "assets.shaders.main.fs." + getRenderName(Void::getRendererAPI()) + PLATFORM_NAME;
		//eastl::vector<uint8_t> fsbytes;
		//vfs.getFile(fName.c_str(), fsbytes);

		//fName = "assets.shaders.main.vs." + getRenderName(Void::getRendererAPI()) + PLATFORM_NAME;
		//eastl::vector<uint8_t> vsbytes;
		//vfs.getFile(fName.c_str(), vsbytes);

		//Void::sp = factory.createShaderProgram(eastl::make_pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>(vsbytes, fsbytes));
		//if (Void::sp == nullptr) {
		//	//Error
		//	//LOG_ERROR("could not create shader program");
		//	
		//	return;
		//}

		//bgfx::Memory* m = nullptr;

		//vfs.getFile("assets.images.logo", m);

		//Void::tex = factory.createTexture(m);

		//if (Void::tex == nullptr) {
		//	
		//	return;
		//}

	}
	void Void::_render() {
		/*Renderer::addFlag(Void::Clear::Color);
		Renderer::addFlag(Void::Clear::Depth);
		Renderer::setClearColor(0x00, 0x00, 0x00);

		VertexBuffer::bind(vb);

		Void::sp->setUniform("u_texture", tex);

		if(sp != nullptr)
			Renderer::submit(sp->getProgramHandle());

		Renderer::touch();*/
	}
	void Void::render(const eastl::shared_ptr<VertexBuffer>& vBuffer, RenderStates& states) {
		static float time = 0.0f;
		time += 0.005f;
		if (!states.shader && !Void::sp)
			return;

		auto& shader = states.shader ? states.shader : Void::sp;

		auto vs = Renderer::getViewSpace();
		width = vs.width;
		height = vs.height;
		float view[16];
		bx::mtxLookAt(view,
			bx::Vec3(0.0f, 0.0f, -4.0f),
			bx::Vec3(0.0f, 0.0f, 0.0f),
			bx::Vec3(0.0f, 1.0f, 0.0f)
		);

		float proj[16];
		bx::mtxProj(proj,
			90.0f,
			vs.width / (float)vs.height,
			0.1f, 100.0f,
			bgfx::getCaps()->homogeneousDepth
		);
		Renderer::setTransform(view, proj);

		float transformMatrix[16];
		bx::mtxSRT(
			transformMatrix,
			states.transform.scale.x, states.transform.scale.y, states.transform.scale.z,
			states.transform.rotation.x, states.transform.rotation.y, states.transform.rotation.z,
			states.transform.position.x, states.transform.position.y, states.transform.position.z
		);
		Renderer::setTransform(transformMatrix);

		bgfx::setState(states.states.sFlag);
		
		Renderer::setVertexBuffer(vBuffer->getNativeHandle());

		if (states.texture && states.texture->getTextureHandle().idx != bgfx::kInvalidHandle) {
			shader->setUniform("u_texture", states.texture);
		}

		shader->setUniform("u_time", time, 0.0f, 0.0f, 0.0f);

		shader->setUniform("u_param", 100.0f, 5.0f, 2.0f, 0.0f);

		Renderer::submit(shader->getProgramHandle());
	}
	void Void::render(const Renderable& renderable) {
		RenderStates states;
		states.shader = sp;
		renderable.render(*this, states);
	}
}
