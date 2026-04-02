#pragma once

#include <cstdint>

#include "Graphics/Renderer.hpp"

namespace sf {
	class Window;
}

namespace mce::gfx::gl {
	class GLRenderer final : public Renderer {
	public:
		GLRenderer(sf::Window& window);
		~GLRenderer() override;

		void renderFrame() override;
	private:
		static void logIfFailed(uint32_t shader);

		sf::Window& window;

		uint32_t vao = 0;

		uint32_t vbo = 0;
		uint32_t colorVbo = 0;
		uint32_t program = 0;
	};
}
