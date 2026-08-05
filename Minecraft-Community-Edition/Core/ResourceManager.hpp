#pragma once
#include <Graphics/RenderFactory.hpp>
#include <Graphics/RenderContext.hpp>
#include <IO/VirtualFileSystem.hpp>
#include "QEventBus.hpp"
#include <EASTL/unordered_map.h>

namespace mce::core {
	class ResourceManager {
	public:
		ResourceManager(QEventBus& qBus, io::VirtualFileSystem& vfs, gfx::RenderFactory& factory, gfx::RenderContext& ctx);
		~ResourceManager();

		QEventBus& getQEventBus();
		gfx::RenderFactory& getRenderFactory();
		gfx::RenderContext& getRenderContext();
		io::VirtualFileSystem& getVirtualFileSystem();

		eastl::shared_ptr<gfx::ShaderProgram> getShader(const eastl::string& vs, const eastl::string& fs);
	private:
		io::VirtualFileSystem& vfs;
		gfx::RenderFactory& factory;
		gfx::RenderContext& ctx;
		QEventBus& qBus;
	};
}