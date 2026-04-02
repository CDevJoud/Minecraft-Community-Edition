#pragma once
#include "Graphics/GraphicsContext.hpp"

namespace mce::gfx::gl {
	class GLGraphicsContext final : public GraphicsContext {
	public:
		GLGraphicsContext();
		~GLGraphicsContext() override;

		eastl::unique_ptr<Renderer> createRenderer(sf::Window& window) override;
	};
}
