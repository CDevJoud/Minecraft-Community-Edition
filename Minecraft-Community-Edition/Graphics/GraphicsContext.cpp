#include "GraphicsContext.hpp"

#include "Common/Assert.hpp"
#include "OpenGL/GLGraphicsContext.hpp"

namespace MCE {
	GraphicsContext::~GraphicsContext() {
	}

	eastl::unique_ptr<GraphicsContext> GraphicsContext::create(API api) {
		switch (api) {
		case API::OpenGL: return eastl::make_unique<GLGraphicsContext>();
		case API::Vulkan: MCE_ASSERT(false, "Vulkan is currently unsupported"); return nullptr;
		}

		MCE_ASSERT(false, "Unknown API");
		return nullptr;
	}
}
