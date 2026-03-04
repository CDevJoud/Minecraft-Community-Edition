#include "GLGraphicsContext.hpp"
#include "GLRenderer.hpp"

namespace MCE {
	GLGraphicsContext::GLGraphicsContext() {
	}

	GLGraphicsContext::~GLGraphicsContext() {
	}

	eastl::unique_ptr<Renderer> GLGraphicsContext::createRenderer(sf::Window& window) {
		return eastl::make_unique<GLRenderer>(window);
	}
}
