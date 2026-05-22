#pragma once
#include "Transform.hpp"
#include "Renderable.hpp"
#include "RenderFactory.hpp"
#include "core/QEventBus.hpp"

namespace mce::gfx {
	class Torus : public Renderable, public Transform {
	public:
		bool create(core::QEventBus& qBus, RenderFactory& factory, float R = 1.0f, float r = 0.0f, int segments = 32, int rings = 16, Color color = Color::Red);

		eastl::shared_ptr<Texture>& getTexture();

		void setTexture(eastl::shared_ptr<Texture>& texture);

		virtual void render(Void& v3d, RenderStates& states) const override;
		bool createPlane(
			core::QEventBus& qBus,
			RenderFactory& factory,
			float width,
			float height,
			int xSegments,
			int ySegments,
			Color color);
	private:
		eastl::shared_ptr<Texture> texture;
		eastl::shared_ptr<VertexBuffer> vb;
		bgfx::VertexLayout layout;
	};
}

