#include "Cube.hpp"
#include "Void.hpp"
#include "Torus.hpp"

namespace mce::gfx {
	Cube::Cube() {
	
	}
	Cube::~Cube() {
	
	}
	bool Cube::create(RenderFactory& factory, sf::Vector2f size, Color color) {
		
		VertexArray vArray;
		// Front (+Z)
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f, 0.5f), Color::Red, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f, 0.5f), Color::Red, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f, 0.5f), Color::Red, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f, 0.5f), Color::Red, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f, 0.5f), Color::Red, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f, 0.5f), Color::Red, sf::Vector2f(0.0f, 0.0f)));
		
		// Back (-Z)
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f,-0.5f), Color::Green, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f,-0.5f), Color::Green, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f,-0.5f), Color::Green, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f,-0.5f), Color::Green, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f,-0.5f), Color::Green, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f,-0.5f), Color::Green, sf::Vector2f(0.0f, 0.0f)));
		
		// Left (-X)
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f,-0.5f), Color::Blue, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f, 0.5f), Color::Blue, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f, 0.5f), Color::Blue, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f,-0.5f), Color::Blue, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f, 0.5f), Color::Blue, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f,-0.5f), Color::Blue, sf::Vector2f(0.0f, 0.0f)));
		
		// Right (+X)
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f, 0.5f), Color::Yellow, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f,-0.5f), Color::Yellow, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f,-0.5f), Color::Yellow, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f, 0.5f), Color::Yellow, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f,-0.5f), Color::Yellow, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f, 0.5f), Color::Yellow, sf::Vector2f(0.0f, 0.0f)));
		
		// Top (+Y)
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f, 0.5f), Color::Cyan, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f, 0.5f), Color::Cyan, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f,-0.5f), Color::Cyan, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f, 0.5f), Color::Cyan, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f, 0.5f,-0.5f), Color::Cyan, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f, 0.5f,-0.5f), Color::Cyan, sf::Vector2f(0.0f, 0.0f)));
		
		// Bottom (-Y)
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f,-0.5f), Color::Magenta, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f,-0.5f), Color::Magenta, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f, 0.5f), Color::Magenta, sf::Vector2f(1.0f, 0.0f)));
		
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f,-0.5f), Color::Magenta, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(Vertex(sf::Vector3f( 0.5f,-0.5f, 0.5f), Color::Magenta, sf::Vector2f(1.0f, 0.0f)));
		vArray.append(Vertex(sf::Vector3f(-0.5f,-0.5f, 0.5f), Color::Magenta, sf::Vector2f(0.0f, 0.0f)));
		
		flags::Buffer f;
		f.addFlag(flags::Buffer::None);
		layout = Vertex::layout();
		vArray.setVertexLayout(layout, sizeof(Vertex));
		vb = factory.createVertexBuffer(vArray, f, "Cube1");

		return (vb != nullptr);
	}
	eastl::shared_ptr<Texture>& Cube::getTexture() {
		// TODO: insert return statement here
		return this->texture;
	}
	void Cube::setTexture(eastl::shared_ptr<Texture>& texture) {
		this->texture = texture;
	}
	void Cube::render(Void& v3d, RenderStates& states) const {
		states.texture = texture;
		states.transform = *this;
		states.states.sFlag = flags::State::None;

		states.states.addFlag(flags::State::WriteRGB)
			.addFlag(flags::State::WriteA)
			.addFlag(flags::State::DepthTestLess)
			.addFlag(flags::State::WriteZ)
			.addFlag(flags::State::CullCCW);

		v3d.render(vb, states);
	}
}

