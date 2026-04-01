#include "GraphicsContext.hpp"

#include <assert.h>
#include "OpenGL/GLGraphicsContext.hpp"

namespace mce {
	GraphicsContext::~GraphicsContext() {
	}

	eastl::unique_ptr<GraphicsContext> GraphicsContext::create(API api) {
		switch (api) {
		case API::OpenGL: return eastl::make_unique<GLGraphicsContext>();
		case API::Vulkan: /*No need of this MCE_ASSERT as it wont output to the console as it will hit the __debugbreak()*/assert(false, "Vulkan is currently unsupported"); return nullptr;
		}

		assert(false, "Unknown API");
		return nullptr;
	}
}
