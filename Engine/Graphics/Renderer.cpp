#include "Renderer.hpp"
#include <libs/bgfx/bgfx.h>
#include <libs/bx/math.h>

namespace dms::gfx {
	Renderer::Renderer(uint16_t viewId, RenderFactory& factory, RenderContext::API api) :
		viewId(viewId), 
		color(0),
		factory(factory),
		backendAPI(api) {

	}
	void Renderer::setViewSpace(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
		bgfx::setViewRect(this->viewId, x, y, width, height);
		viewSpace = { x, y, width, height };
	}
	void Renderer::setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->color = (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a),
		bgfx::setViewClear(this->viewId, static_cast<uint16_t>(this->clearFlags), this->color);
	}

	void Renderer::setVertexBuffer(const eastl::shared_ptr<VertexBuffer>& vb) {
		Renderer::setVertexBuffer(vb->getNativeHandle());
	}

	void Renderer::setIndexBuffer(IndexBufferHandle handle) {
		bgfx::setIndexBuffer(handle);
	}

	void Renderer::setIndexBuffer(const eastl::shared_ptr<IndexBuffer>& ib) {
		Renderer::setIndexBuffer(ib->getNativeHandle());
	}

	void Renderer::setVertexBuffer(VertexBufferHandle handle) {
		bgfx::setVertexBuffer(0, handle);
	}

	void Renderer::setTransform(const void* view, const void* proj) {
		bgfx::setViewTransform(this->viewId, view, proj);
	}

	void Renderer::setTransform(const void* modelViewProj) {
		bgfx::setTransform(modelViewProj);
	}

    void Renderer::setTexture(uint8_t stage, bgfx::UniformHandle _sampler, bgfx::TextureHandle _handle, uint32_t flags) {
		bgfx::setTexture(stage, _sampler, _handle, flags);
	}

	void Renderer::setState(uint64_t state) {
		bgfx::setState(state);
	}

	void Renderer::setState(flags::State state) {
		Renderer::setState(static_cast<uint64_t>(state.sFlag));
	}

	void Renderer::submit(ProgramHandle program) {
		bgfx::submit(Renderer::viewId, program);
	}

	void Renderer::submit(const eastl::shared_ptr<ShaderProgram>& sp, uint32_t depth) {
		bgfx::submit(Renderer::viewId, sp->getProgramHandle(), depth);
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
	core::Rect<uint16_t> Renderer::getViewSpace() const {
		return Renderer::viewSpace;
	}
}
