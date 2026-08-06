#include "Vertex.hpp"

namespace dms::gfx {
	Vertex::Vertex() {
	
	}
	Vertex::Vertex(sf::Vector3f position, Color color, sf::Vector2f texCoords) :
		position(position), color(color), texCoords(texCoords) {
		
	}
	bgfx::VertexLayout Vertex::layout() {
		bgfx::VertexLayout lay;
		lay.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
		return lay;
	}
}
