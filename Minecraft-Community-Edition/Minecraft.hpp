#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/string.h>
#include "SFML/Window/Window.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Graphics/Renderer.hpp"
#include "Core/QEventBus.hpp"

namespace mce {
	namespace gfx {
		class GraphicsContext;
		class Renderer;
	}
	
	using core::QEventBus;
	using gfx::GraphicsContext;
	using gfx::Renderer;

	class Minecraft {
	public:
		Minecraft(const eastl::string_view& profileName);
		~Minecraft();


		int run();
	private:
		int initInstance();

		void handleEvents();

		inline void translateEventAndDispatch(const std::optional<sf::Event> event);

		std::string getLogFileName();;

		void setupLogging();

		sf::Window window;

		eastl::unique_ptr<GraphicsContext> graphicsContext;
		eastl::unique_ptr<Renderer> renderer;
		QEventBus qBus;
	};
}
