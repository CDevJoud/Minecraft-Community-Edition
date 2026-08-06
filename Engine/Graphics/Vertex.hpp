#pragma once
#include <Core/Vector3.hpp>
#include <Core/Vector2.hpp>
#include "Color.hpp"
#include "../libs/bgfx/bgfx.h"

namespace dms::gfx {
	class Vertex {
	public:
		Vertex();
		Vertex(sf::Vector3f position, Color color, sf::Vector2f texCoords);
		sf::Vector3f position;
		Color color;
		sf::Vector2f texCoords;

		static bgfx::VertexLayout layout();
	};
}
