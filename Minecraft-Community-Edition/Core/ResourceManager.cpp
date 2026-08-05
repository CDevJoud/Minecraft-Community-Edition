#include "ResourceManager.hpp"
#include <Platform.hpp>
#include <format>
#pragma warning(disable:4996)
#define LOG_ERROR(...) qBus.post(event::Log(event::Log::ERROR, std::format(__VA_ARGS__)));
namespace mce::core {
	using gfx::ShaderProgram;
	using eastl::string;
	using eastl::shared_ptr;
	using eastl::vector;
	using bgfx::Memory;

	string getRenderName(gfx::RenderContext::API api) {
		switch (api) {
		case mce::gfx::RenderContext::API::Noop:
			break;
		case mce::gfx::RenderContext::API::Agc:
			break;
		case mce::gfx::RenderContext::API::Direct3D11:
			return ".d3d11_";
		case mce::gfx::RenderContext::API::Direct3D12:
			return ".d3d12_";
		case mce::gfx::RenderContext::API::Gnm:
			break;
		case mce::gfx::RenderContext::API::Metal:
			return ".metal_";
		case mce::gfx::RenderContext::API::Nvn:
			break;
		case mce::gfx::RenderContext::API::OpenGLES:
			break;
		case mce::gfx::RenderContext::API::OpenGL:
			return ".gl_";
		case mce::gfx::RenderContext::API::Vulkan:
			return ".vulkan_";
		case mce::gfx::RenderContext::API::WebGPU:
			break;
		case mce::gfx::RenderContext::API::Count:
			break;
		default:
			break;
		}
	}
	ResourceManager::ResourceManager(QEventBus& qBus, io::VirtualFileSystem& vfs, gfx::RenderFactory& factory, gfx::RenderContext& ctx) : 
		qBus(qBus),
		vfs(vfs),
		factory(factory),
		ctx(ctx) {
	
	}
	ResourceManager::~ResourceManager() {
	
	}
	QEventBus& ResourceManager::getQEventBus() {
		return ResourceManager::qBus;
	}

	gfx::RenderFactory& ResourceManager::getRenderFactory() {
		return ResourceManager::factory;
	}

	gfx::RenderContext& ResourceManager::getRenderContext() {
		return ResourceManager::ctx;
	}

    io::VirtualFileSystem& ResourceManager::getVirtualFileSystem() {
		return ResourceManager::vfs;
    }

	shared_ptr<ShaderProgram> ResourceManager::getShader(const string& vs, const string& fs) {
		vector<uint8_t> vsMem;
		auto vsFileTag = vs + getRenderName(ctx.getRenderAPI()) + PLATFORM_NAME;
		if (!vfs.getFile(vsFileTag, vsMem)) {
			LOG_ERROR("ResourceManager: Couldn't get the vertex shader. File tag \"{}\" as it doesn't exist!", vsFileTag.c_str());
			return nullptr;
		}

		vector<uint8_t> fsMem;
		auto fsFileTag = fs + getRenderName(ctx.getRenderAPI()) + PLATFORM_NAME;
		if (!vfs.getFile(fsFileTag, fsMem)) {
			LOG_ERROR("ResourceManager: Couldn't get the fragment shader. File tag \"{}\" as it doesn't exist!", fsFileTag.c_str());
			return nullptr;
		}
		shared_ptr<ShaderProgram> sp = factory.createShaderProgram(eastl::make_pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>(vsMem, fsMem));
		if (sp == nullptr) {
			LOG_ERROR("ResourceManager: Couldn't create shader program with {} & {}", vsFileTag, fsFileTag);
			return nullptr;
		}
		return sp;
	}
}

