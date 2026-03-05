#pragma once
#include <EASTL/unique_ptr.h>
#include "SFML/Window/Window.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Graphics/Renderer.hpp"

namespace MCE {
	class GraphicsContext;
	class Renderer;

	class Minecraft {
	public:
		Minecraft();

		void run();
	private:
		sf::Window window;

		eastl::unique_ptr<GraphicsContext> graphicsContext;
		eastl::unique_ptr<Renderer> renderer;
	};
}
