#include "Minecraft.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"

#include "SFML/Window/VideoMode.hpp"

namespace mce {
	Minecraft::Minecraft() : window(sf::VideoMode({1280, 720}), "Minecraft: Community Edition") {
		//MCE_INFO("Starting MCE");
		graphicsContext = GraphicsContext::create(GraphicsContext::API::OpenGL);
		renderer = graphicsContext->createRenderer(window);
		//MCE_INFO("Done");
	}

	Minecraft::~Minecraft() {
		//MCE_INFO("Shutting down");
	}

	void Minecraft::run() {
		while (true) {
			while (const auto event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>()) {
					return;
				}
			}

			renderer->renderFrame();
			window.display();
		}
	}
}
