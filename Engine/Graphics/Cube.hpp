#pragma once
#include "Transform.hpp"
#include "Renderable.hpp"
#include "RenderFactory.hpp"

namespace dms::gfx {
	class Cube : public Transform, public Renderable {
	public:
		Cube();
		~Cube();

		bool create(RenderFactory& factory, sf::Vector2f size, Color color);

		eastl::shared_ptr<Texture>& getTexture();

		void setTexture(eastl::shared_ptr<Texture>& texture);

		virtual void render(Void& v3d, RenderStates& states) const override;

	private:
		eastl::shared_ptr<Texture> texture;
		eastl::shared_ptr<VertexBuffer> vb;
		bgfx::VertexLayout layout;
	};
}

