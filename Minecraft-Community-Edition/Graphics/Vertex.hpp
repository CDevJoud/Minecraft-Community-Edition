#pragma once
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <bgfx/bgfx.h>

namespace mce::gfx {
	class Vertex {
	public:
		Vertex();
		Vertex(sf::Vector3f position, sf::Color color, sf::Vector2f texCoords);
		sf::Vector3f position;
		sf::Color color;
		sf::Vector2f texCoords;

		static bgfx::VertexLayout layout();
	};
}
