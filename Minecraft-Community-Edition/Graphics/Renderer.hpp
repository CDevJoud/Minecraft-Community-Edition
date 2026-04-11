#pragma once
#include "IRenderer.hpp"
#include "RenderFactory.hpp"
#include "RenderContext.hpp"

namespace mce::gfx {
	class Renderer : public IRenderer {
	public:
		Renderer(uint16_t viewId, RenderFactory& factory, RenderContext::API api);
		~Renderer() = default;
		virtual void setViewSpace(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

		virtual void setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) override;

		virtual void setVertexBuffer(VertexBufferHandle handle) override;

		virtual void touch();

		RenderFactory& getFactory();
		RenderContext::API getRendererAPI();
	private:
		RenderFactory& factory;
		uint32_t color;
		uint16_t viewId;
		RenderContext::API backendAPI;
	};
}

