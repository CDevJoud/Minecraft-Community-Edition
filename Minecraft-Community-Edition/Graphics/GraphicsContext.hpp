#pragma once
#include "EASTL/unique_ptr.h"

namespace sf {
	class Window;
}

namespace MCE {
	class Renderer;

	class GraphicsContext {
	public:
		enum class API {
			OpenGL,
			Vulkan,
		};

		virtual ~GraphicsContext();
		virtual eastl::unique_ptr<Renderer> createRenderer(sf::Window& window) = 0;

		static eastl::unique_ptr<GraphicsContext> create(API api);
	};
}
