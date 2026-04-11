#include "Renderer.hpp"
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace mce::gfx {
	Renderer::Renderer(uint16_t viewId, RenderFactory& factory, RenderContext::API api) :
		viewId(viewId), 
		color(0),
		factory(factory),
		backendAPI(api) {

	}
	void Renderer::setViewSpace(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
		bgfx::setViewRect(this->viewId, x, y, width, height);
	}
	void Renderer::setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->color = (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a),
		bgfx::setViewClear(this->viewId, static_cast<uint16_t>(this->clearFlags), this->color);
	}

	void Renderer::setVertexBuffer(VertexBufferHandle handle) {
		bgfx::setVertexBuffer(0, handle);
	}

	void Renderer::touch() {
		bgfx::touch(this->viewId);
	}
	RenderFactory& Renderer::getFactory() {
		return Renderer::factory;
	}
	RenderContext::API Renderer::getRendererAPI() {
		return Renderer::backendAPI;
	}
}
