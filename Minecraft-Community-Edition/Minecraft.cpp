#include "Minecraft.hpp"

#include "SFML/Window/VideoMode.hpp"

namespace MCE {
	Minecraft::Minecraft() : window(sf::VideoMode({1280, 720}), "Minecraft: Community Edition") {
		graphicsContext = GraphicsContext::create(GraphicsContext::API::OpenGL);
		renderer = graphicsContext->createRenderer(window);
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
