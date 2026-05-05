#include "Torus.hpp"
#include "libs/bgfx/bgfx.h"
#include "Void.hpp"
#include <format>

namespace mce::gfx {

	void Torus::render(Void& v3d, RenderStates& states) const {
		states.texture = texture;
		states.transform = *this;
		states.states.sFlag = flags::State::None;

		states.states.addFlag(flags::State::WriteRGB)
			.addFlag(flags::State::WriteA)
			.addFlag(flags::State::DepthTestLess)
			.addFlag(flags::State::WriteZ)
			.addFlag(flags::State::CullCCW)
			//.addFlag(flags::State::PrimitiveTypeLineStrip);
			;
		v3d.render(vb, states);


	}

	bool Torus::create(core::QEventBus&	qBus, RenderFactory& factory, float R, float r, int segments, int rings, Color color) {
		VertexArray vArray;
		vArray.setVertexLayout(Vertex::layout(), sizeof(Vertex));
		for (int i = 0; i < segments; ++i) {
			float u0 = (float)i / segments * 2.0f * 3.14159265f;
			float u1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;

			for (int j = 0; j < rings; ++j) {
				float v0 = (float)j / rings * 2.0f * 3.14159265f;
				float v1 = (float)(j + 1) / rings * 2.0f * 3.14159265f;

				// Helper lambda to compute position
				auto torusPoint = [&](float u, float v) {
					float x = (R + r * cos(v)) * cos(u);
					float y = (R + r * cos(v)) * sin(u);
					float z = r * sin(v);
					return sf::Vector3f(x, y, z);
					};

				sf::Vector3f p0 = torusPoint(u0, v0);
				sf::Vector3f p1 = torusPoint(u1, v0);
				sf::Vector3f p2 = torusPoint(u1, v1);
				sf::Vector3f p3 = torusPoint(u0, v1);

				// Simple UVs
				sf::Vector2f uv0((float)i / segments, (float)j / rings);
				sf::Vector2f uv1((float)(i + 1) / segments, (float)j / rings);
				sf::Vector2f uv2((float)(i + 1) / segments, (float)(j + 1) / rings);
				sf::Vector2f uv3((float)i / segments, (float)(j + 1) / rings);

				/*sf::Vector2f uv0(0.0f, 0.0f);
				sf::Vector2f uv1(1.0f, 0.0f);
				sf::Vector2f uv2(1.0f, 1.0f);
				sf::Vector2f uv3(0.0f, 1.0f);*/

				// Triangle 1
				vArray.append(Vertex(p0, color, uv0));
				vArray.append(Vertex(p1, color, uv1));
				vArray.append(Vertex(p2, color, uv2));

				/*qBus.post(event::Log(event::Log::INFO,
					std::format("Triangle 1: p0:{}, {} p1:{}, {} p2:{}, {} uv0:{}, {} uv1:{}, {} uv2:{}, {}",
						p0.x, p0.y,
						p1.x, p1.y,
						p2.x, p2.y,
						uv0.x, uv0.y,
						uv1.x, uv1.y,
						uv2.x, uv2.y
					),
					"bgfx",
					std::source_location::current()
				));*/

				// Triangle 2
				vArray.append(Vertex(p0, color, uv0));
				vArray.append(Vertex(p2, color, uv2));
				vArray.append(Vertex(p3, color, uv3));

				/*qBus.post(event::Log(event::Log::INFO,
					std::format("Triangle 2: p0:{}, {} p1:{}, {} p2:{}, {} uv0:{}, {} uv1:{}, {} uv2:{}, {}",
						p0.x, p0.y,
						p1.x, p1.y,
						p2.x, p2.y,
						uv0.x, uv0.y,
						uv1.x, uv1.y,
						uv2.x, uv2.y
					),
					"bgfx",
					std::source_location::current()
				));*/
			}
		}

		flags::Buffer fl;
		fl.addFlag(flags::Buffer::None);
		vb = factory.createVertexBuffer(vArray, fl);

		return (vb != nullptr);
	}

	eastl::shared_ptr<Texture>& Torus::getTexture() {
		return texture;
	}

	void Torus::setTexture(eastl::shared_ptr<Texture>& texture) {
		this->texture = texture;
	}
}
