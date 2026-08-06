#include "RmlUI_Renderer_bgfx.hpp"
#include "RmlUi/Core/Log.h"
#include "Graphics/BgfxRenderContext.hpp"
#include <bx/math.h>

namespace dms::ui::priv {
	bgfx::VertexLayout RmlVertex::layout() {
		static bgfx::VertexLayout vLayout;
		vLayout.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
		return vLayout;
	}

	// Convert Rml::Matrix4f (column-major, same as glm) to float[16]
	static void Matrix4ToFloat16(const Rml::Matrix4f& m, float out[16]) {
		// Rml::Matrix4f stores data column-major, same layout as bgfx expects
		std::memcpy(out, m.data(), 16 * sizeof(float));
	}

	RenderInterface_bgfx::RenderInterface_bgfx(core::ResourceManager& rm) :
		gfx::Renderer(0, rm.getRenderFactory(), rm.getRenderContext().getRenderAPI()),
		rsrcMgr(rm),
		programs({}),
		activeProgram(RmlProgramId::None),
		shader(nullptr),
		nextGeometryId(1),
		geometries({}),
		nextFilterId(1),
		filters({}),
		nextShaderId(1),
		shaders({}),
		nextTextureId(1),
		transform(Rml::Matrix4f::Identity()),
		projection(Rml::Matrix4f::Identity()),
		scissorEnabled(false),
		scissorRegion({}),
		clipMaskEnabled(false),
		stencilRef(0), // current stencil reference value
		stencilValue(1), // value written during clip-mask
		currentViewId(0),
		baseView(0), // first view allocated this frame
		viewDirty(true),
		lastBoundTexture(nullptr),
		viewportWidth(0), viewportHeight(0), viewportOffsetX(0), viewportOffsetY(0),
		valid(false),
		drawOrder(0) {
		programs.resize(static_cast<eastl_size_t>(RmlProgramId::Count));

		this->valid = this->loadPrograms();

		if (this->valid) {
			// Build a fullscreen quad [-1,1] covering NDC space.
			// Uses tex_coord for UV.
			struct FSVert { float x, y; uint32_t col; float u, v; };
			static const FSVert verts[] = {
				{-1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f},
				{ 1.f, -1.f, 0xFFFFFFFF, 1.f, 0.f},
				{ 1.f,  1.f, 0xFFFFFFFF, 1.f, 1.f},
				{-1.f,  1.f, 0xFFFFFFFF, 0.f, 1.f},
			};
			static const int indices[] = { 0, 1, 2, 0, 2, 3 };

			this->fullscreenQuad = CompileGeometry(
				{ reinterpret_cast<const Rml::Vertex*>(verts), 4 },
				{ indices, 6 }
			);
		}
		auto api = rsrcMgr.getRenderContext().getRenderAPI();
		this->frameBufferOriginBottomLeft = (api == gfx::RenderContext::API::OpenGL || api == gfx::RenderContext::API::OpenGLES);
	}
	RenderInterface_bgfx::~RenderInterface_bgfx() {

	}
	void RenderInterface_bgfx::setViewport(int w, int h, int ox, int oy) {
		viewportWidth = w;
		viewportHeight = h;
		viewportOffsetX = ox;
		viewportOffsetY = oy;
		projection = makeProjection();
	}
	void RenderInterface_bgfx::beginFrame(uint16_t viewId) {
		this->drawOrder = this->stencilRef = 0;
		this->stencilValue = 1;
		this->clipMaskEnabled = false;
		this->scissorEnabled = false;
		this->activeProgram = RmlProgramId::None;
		this->lastBoundTexture = nullptr;
		this->transform = this->makeProjection();

		//this->layers.beginFrame(*this, viewportWidth, viewportHeight);

		baseView = viewId;

	}

	void RenderInterface_bgfx::endFrame() {

	}
	Rml::Matrix4f RenderInterface_bgfx::makeProjection() const {
		const float L = static_cast<float>(viewportOffsetX);
		const float R = L + static_cast<float>(viewportWidth);
		const float T = static_cast<float>(viewportOffsetY);
		const float B = T + static_cast<float>(viewportHeight);

		Rml::Matrix4f proj = Rml::Matrix4f::Identity();

		float* m = proj.data();

		m[0] = 2.f / (R - L);
		m[5] = 2.f / (T - B);
		m[10] = 1.f;

		m[12] = -(R + L) / (R - L);
		m[13] = -(T + B) / (T - B);
		m[14] = 0.f;
		m[15] = 1.f;

		return proj;
	}
	Rml::CompiledGeometryHandle RenderInterface_bgfx::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) {
		BgfxCompiledGeometry geo;

		gfx::RenderFactory& factory = rsrcMgr.getRenderFactory();
		const bgfx::Memory* vMem = bgfx::copy(
			vertices.data(),
			static_cast<uint32_t>(vertices.size() * sizeof(Rml::Vertex))
		);

		gfx::flags::Buffer fl;
		fl.addFlag(gfx::flags::Buffer::None);
		geo.vb = factory.createVertexBuffer(vMem, RmlVertex::layout(), fl, "RmlUIVertex");

		eastl::vector<uint32_t> idx32(indices.size());
		for (size_t i = 0; i < indices.size(); ++i) {
			idx32[i] = static_cast<uint32_t>(indices[i]);
		}

		const bgfx::Memory* imem = bgfx::copy(idx32.data(), static_cast<uint32_t>(idx32.size() * sizeof(uint32_t)));
		gfx::flags::Buffer iFlags;
		iFlags.addFlag(gfx::flags::Buffer::Index32);
		geo.ib = factory.createIndexBuffer(imem, iFlags);

		geo.num_indices = static_cast<int>(indices.size());

		Rml::CompiledGeometryHandle handle = this->nextGeometryId++;
		this->geometries[handle] = geo;
		return handle;
	}
	void RenderInterface_bgfx::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) {
		submitGeometry(handle, translation, texture);
	}
	void RenderInterface_bgfx::ReleaseGeometry(Rml::CompiledGeometryHandle handle) {
		auto it = this->geometries.find(handle);
		if (it == geometries.end()) return;
		geometries.erase(it);
	}
	Rml::TextureHandle RenderInterface_bgfx::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) {
		return Rml::TextureHandle();
	}
	Rml::TextureHandle RenderInterface_bgfx::GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions) {

		const bgfx::Memory* mem = bgfx::copy(source_data.data(), static_cast<uint32_t>(source_data.size()));
		auto& factory = rsrcMgr.getRenderFactory();
		eastl::shared_ptr<gfx::Texture> tex = factory.createTexture(
			sf::Vector2i(source_dimensions.x, source_dimensions.y),
			false, 1, bgfx::TextureFormat::RGBA8,
			BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP,
			mem
		);
		if (tex == nullptr) {
			return {};
		}
		// when the tex goes out of scope the reference count decreement and it will release the texture but Rml still want to access it
		// so we insert it in a container that increement the reference count
		generatedTextures.push_back(tex);
		return static_cast<Rml::TextureHandle>(tex->getTextureHandle().idx);
			
	}
	void RenderInterface_bgfx::ReleaseTexture(Rml::TextureHandle texture_handle) {
		auto it = eastl::find_if(generatedTextures.begin(), generatedTextures.end(), [&texture_handle](const eastl::shared_ptr<gfx::Texture>& p) {
			return p->getTextureHandle().idx == static_cast<uint16_t>(texture_handle);
			});

		generatedTextures.erase(it);
	}
	void RenderInterface_bgfx::EnableScissorRegion(bool enable) {
		this->scissorEnabled = enable;
	}
	void RenderInterface_bgfx::SetScissorRegion(Rml::Rectanglei region) {
		this->scissorRegion = region;
	}
	void RenderInterface_bgfx::EnableClipMask(bool enable) {
		this->clipMaskEnabled = enable;
	}
	void RenderInterface_bgfx::RenderToClipMask(Rml::ClipMaskOperation operation, Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation) {
		auto it = this->geometries.find(geometry);
		if (it == this->geometries.end()) return;

		const auto& geo = it->second;

		auto& creationShader = this->programs[static_cast<int>(RmlProgramId::Creation)];

		if (creationShader == nullptr) {
			return;
		}

		useProgram(RmlProgramId::Creation);

		// Sets the transformation uniform
		auto translate_mat = Rml::Matrix4f::Translate(translation.x, translation.y, 0.0f);
		auto mvp = projection * transform * translate_mat;

		float outproj[16];

		Matrix4ToFloat16(mvp, outproj);

		if (creationShader != nullptr) {
			creationShader->setUniform("u_transform", outproj);
		}

		Renderer::setVertexBuffer(geo.vb);
		Renderer::setIndexBuffer(geo.ib);

		uint64_t state = BGFX_STATE_MSAA;
		bgfx::setState(state);

		if (scissorEnabled) {
			bgfx::setScissor(
				uint16_t(this->scissorRegion.Left()),
				uint16_t(this->scissorRegion.Top()),
				uint16_t(this->scissorRegion.Width()),
				uint16_t(this->scissorRegion.Height())
			);
		}

		uint32_t stencil = 0;

		switch (operation) {
		case Rml::ClipMaskOperation::Set:
		{
			stencilRef = stencilValue++;

			stencil = 0
				| BGFX_STENCIL_TEST_ALWAYS
				| BGFX_STENCIL_FUNC_REF(stencilRef)
				| BGFX_STENCIL_FUNC_RMASK(0xFF)
				| BGFX_STENCIL_OP_FAIL_S_ZERO
				| BGFX_STENCIL_OP_FAIL_Z_ZERO
				| BGFX_STENCIL_OP_PASS_Z_REPLACE;
			break;
		}
		case Rml::ClipMaskOperation::SetInverse:
		{
			stencilRef = stencilValue++;

			// Fullscreen quad to fill stencil
			{
				auto fsIt = geometries.find(fullscreenQuad);
				if (fsIt != geometries.end()) {
					const auto& fsGeo = fsIt->second;

					float identity[16];
					bx::mtxIdentity(identity);

					Renderer::setVertexBuffer(fsGeo.vb);
					Renderer::setIndexBuffer(fsGeo.ib);
					bgfx::setState(BGFX_STATE_MSAA);

					uint32_t fillStencil = 0
						| BGFX_STENCIL_TEST_ALWAYS
						| BGFX_STENCIL_FUNC_REF(stencilRef)
						| BGFX_STENCIL_FUNC_RMASK(0xFF)
						| BGFX_STENCIL_OP_FAIL_S_REPLACE
						| BGFX_STENCIL_OP_FAIL_Z_REPLACE
						| BGFX_STENCIL_OP_PASS_Z_REPLACE;

					bgfx::setStencil(fillStencil);

					creationShader->setUniform("u_transform", identity);
					bgfx::submit(currentViewId, creationShader->getProgramHandle(), drawOrder++);
				}
			}
			auto translate_mat = Rml::Matrix4f::Translate(translation.x, translation.y, 0.0f);
			auto mvp = projection * transform * translate_mat;

			float outproj[16];

			Matrix4ToFloat16(mvp, outproj);
			creationShader->setUniform("u_transform", outproj);
			bgfx::setVertexBuffer(0, geo.vb->getNativeHandle());
			bgfx::setIndexBuffer(geo.ib->getNativeHandle());
			bgfx::setState(BGFX_STATE_MSAA);

			stencil = 0
				| BGFX_STENCIL_TEST_ALWAYS
				| BGFX_STENCIL_FUNC_REF(0)
				| BGFX_STENCIL_FUNC_RMASK(0xFF)
				| BGFX_STENCIL_OP_FAIL_S_REPLACE
				| BGFX_STENCIL_OP_FAIL_Z_REPLACE
				| BGFX_STENCIL_OP_PASS_Z_REPLACE;
			break;
		}
		case Rml::ClipMaskOperation::Intersect:
		{
			uint8_t old_ref = stencilRef;
			stencilRef = stencilValue++;

			if (stencilRef == old_ref + 1) {
				stencil = 0
					| BGFX_STENCIL_TEST_EQUAL
					| BGFX_STENCIL_FUNC_REF(old_ref)
					| BGFX_STENCIL_FUNC_RMASK(0xFF)
					| BGFX_STENCIL_OP_FAIL_S_KEEP
					| BGFX_STENCIL_OP_FAIL_Z_KEEP
					| BGFX_STENCIL_OP_PASS_Z_INCR;
			}
			else {
				stencil = 0
					| BGFX_STENCIL_TEST_LEQUAL
					| BGFX_STENCIL_FUNC_REF(stencilRef)
					| BGFX_STENCIL_FUNC_RMASK(0xFF)
					| BGFX_STENCIL_OP_FAIL_S_KEEP
					| BGFX_STENCIL_OP_FAIL_Z_KEEP
					| BGFX_STENCIL_OP_PASS_Z_REPLACE;
			}
			break;
		}
		}

		if (scissorEnabled) {
			bgfx::setScissor(
				uint16_t(scissorRegion.Left()),
				uint16_t(scissorRegion.Top()),
				uint16_t(scissorRegion.Width()),
				uint16_t(scissorRegion.Height())
			);
		}
		bgfx::setStencil(stencil);
		bgfx::submit(currentViewId, creationShader->getProgramHandle(), drawOrder++);
	}
	void RenderInterface_bgfx::SetTransform(const Rml::Matrix4f* transform) {}
	Rml::LayerHandle RenderInterface_bgfx::PushLayer() {
		return Rml::LayerHandle();
	}
	void RenderInterface_bgfx::CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode, Rml::Span<const Rml::CompiledFilterHandle> filters) {}
	void RenderInterface_bgfx::PopLayer() {}
	Rml::TextureHandle RenderInterface_bgfx::SaveLayerAsTexture() {
		return Rml::TextureHandle();
	}
	Rml::CompiledFilterHandle RenderInterface_bgfx::SaveLayerAsMaskImage() {
		return Rml::CompiledFilterHandle();
	}
	Rml::CompiledFilterHandle RenderInterface_bgfx::CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) {
		return Rml::CompiledFilterHandle();
	}
	void RenderInterface_bgfx::ReleaseFilter(Rml::CompiledFilterHandle filter) {}
	Rml::CompiledShaderHandle RenderInterface_bgfx::CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) {
		return Rml::CompiledShaderHandle();
	}
	void RenderInterface_bgfx::RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation, Rml::TextureHandle texture) {}
	void RenderInterface_bgfx::ReleaseShader(Rml::CompiledShaderHandle effect_handle) {}
	bool RenderInterface_bgfx::loadPrograms() {
		struct ProgramDef { RmlProgramId id; const char* vs; const char* fs; };
		static const ProgramDef defs[] = {
			{ RmlProgramId::Color,       "assets.shaders.rmlui.vs"			  , "assets.shaders.rmlui_color.fs"},
			{ RmlProgramId::Texture,     "assets.shaders.rmlui.vs"			  , "assets.shaders.rmlui_texture.fs"},
			{ RmlProgramId::Passthrough, "assets.shaders.rmlui_passthrough.vs", "assets.shaders.rmlui_passthrough.fs" },
			{ RmlProgramId::BlendMask,   "assets.shaders.rmlui_passthrough.vs", "assets.shaders.rmlui_blendmask.fs"  },
			{ RmlProgramId::Blur,        "assets.shaders.rmlui_passthrough.vs", "assets.shaders.rmlui_blur.fs"        },
			{ RmlProgramId::DropShadow,  "assets.shaders.rmlui_passthrough.vs", "assets.shaders.rmlui_dropshadow.fs" },
			{ RmlProgramId::ColorMatrix, "assets.shaders.rmlui_passthrough.vs", "assets.shaders.rmlui_colormatrix.fs"},
			{ RmlProgramId::Creation,    "assets.shaders.rmlui.vs"			  , "assets.shaders.rmlui_creation.fs"    },
			{ RmlProgramId::Gradient,    "assets.shaders.rmlui.vs"			  , "assets.shaders.rmlui_gradient.fs"    }
		};

		for (auto& def : defs) {
			eastl::shared_ptr<gfx::ShaderProgram> sp = rsrcMgr.getShader(def.vs, def.fs);
			if (!sp) {
				Rml::Log::Message(Rml::Log::LT_ERROR, "RmlUi-BGFX: Failed to load program %s, / %s", def.vs, def.fs);
				destroyPrograms();
				return false;
			}
			programs[static_cast<eastl_size_t>(def.id)] = sp;
		}
		return true;
	}

	void RenderInterface_bgfx::destroyPrograms() {
		for (int i = 0; i < static_cast<int>(RmlProgramId::Count); ++i) {
			programs.pop_back();
		}
	}
	bgfx::ViewId RenderInterface_bgfx::allocateView() {
		auto bgfxCtx = reinterpret_cast<gfx::BgfxRenderContext*>(&rsrcMgr.getRenderContext());
		return bgfxCtx->nextViewId++;
	}
	void RenderInterface_bgfx::setupView(bgfx::ViewId view, const BgfxFrameBuffer& fb) {
		bgfx::setViewName(view, std::string("RmlUi_Layer: " + std::to_string(view)).c_str());
		bgfx::setViewFrameBuffer(view, fb.fb->getNativeHandle());
		bgfx::setViewRect(view, 0, 0, uint16_t(fb.width), uint16_t(fb.height));
		bgfx::setViewClear(view, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0x00000000, 1.0f, 0);

		float proj[16];
		Matrix4ToFloat16(this->projection, proj);
		float identity[16];
		bx::mtxIdentity(identity);
		bgfx::setViewTransform(view, identity, proj);

		bgfx::touch(view);
	}
	void RenderInterface_bgfx::setupViewSpace(bgfx::ViewId view) {
		const auto& topLayer = layers.getTopLayer();
		bgfx::setViewRect(view, 0, 0, uint16_t(topLayer.width), uint16_t(topLayer.height));
	}

	void RenderInterface_bgfx::ensureView() {
		if (!this->viewDirty) return;
		viewDirty = false;
	}


	void RenderInterface_bgfx::useProgram(RmlProgramId id) {
		activeProgram = id;
	}
	void RenderInterface_bgfx::submitGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture, RmlProgramId program_override) {
		auto it = this->geometries.find(handle);
		if (it == this->geometries.end()) return;

		const auto& geo = it->second;

		RmlProgramId prog = program_override;
		if (prog == RmlProgramId::None) {
			if (texture == 0) {
				prog = RmlProgramId::Color;
			}
			else if (texture == TexturePostprocess) {
				prog = activeProgram;
			}
			else {
				prog = RmlProgramId::Texture;
			}
		}

		useProgram(prog);

		auto& shader = programs[static_cast<int>(prog)];
		if (!shader) return;
		setTransformUniform(translation);

		auto translate_mat = Rml::Matrix4f::Translate(translation.x, translation.y, 0.0f);
		auto mvp = projection * transform * translate_mat;

		float outproj[16];

		Matrix4ToFloat16(mvp, outproj);

		if (texture != 0 && texture != TexturePostprocess) {
			eastl::shared_ptr<gfx::Texture> tex = nullptr;
			if (texture == TextureEnableWithoutBinding) {
				tex = lastBoundTexture;
			}
			else {
				auto it = eastl::find_if(generatedTextures.begin(), generatedTextures.end(), [&texture](const eastl::shared_ptr<gfx::Texture>& p) {
					return p->getTextureHandle().idx == texture;
					});
				if (it != generatedTextures.end()) {
					tex = *it;
				}
				lastBoundTexture = tex;
			}
		}

		//auto state = buildBaseState();
		auto state = 0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_MSAA;

		if (prog == RmlProgramId::Color || texture == 0) {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA);
		}
		else {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA);
		}

		// Scissor
		if (scissorEnabled) {
			bgfx::setScissor(
				uint16_t(scissorRegion.Left()),
				uint16_t(scissorRegion.Top()),
				uint16_t(scissorRegion.Width()),
				uint16_t(scissorRegion.Height())
			);
		}

		// Stencil
		if (clipMaskEnabled) {
			uint32_t stencil = buildStencilState();
			bgfx::setStencil(stencil);
		}


		bgfx::setState(state);

		Renderer::setVertexBuffer(geo.vb);
		Renderer::setIndexBuffer(geo.ib);

		if (lastBoundTexture != nullptr)
			shader->setUniform("s_texture0", lastBoundTexture);

		shader->setUniform("u_transform", outproj);

		Renderer::submit(shader, drawOrder++);
	}
	gfx::flags::State RenderInterface_bgfx::buildBaseState() const {
		static gfx::flags::State sf;
		sf.addFlag(gfx::flags::State::WriteRGB)
			.addFlag(gfx::flags::State::WriteA)
			.addFlag(gfx::flags::State::MSAA);
		return sf;
	}
	uint32_t RenderInterface_bgfx::buildStencilState() const {
		return 0
			| BGFX_STENCIL_TEST_EQUAL
			| BGFX_STENCIL_FUNC_REF(stencilRef)
			| BGFX_STENCIL_FUNC_RMASK(0xFF)
			| BGFX_STENCIL_OP_FAIL_S_KEEP
			| BGFX_STENCIL_OP_FAIL_Z_KEEP
			| BGFX_STENCIL_OP_PASS_Z_KEEP;
	}
	void RenderInterface_bgfx::setTransformUniform(Rml::Vector2f translation) {}

	BgfxFrameBuffer RenderInterface_bgfx::createFrameBuffer(int w, int h, bool with_depth_stencil) {
		auto& factory = Renderer::getFactory();
		
		BgfxFrameBuffer fb;
		fb.width = w;
		fb.height = h;
		
		fb.color = factory.createTexture({ uint16_t(w), uint16_t(h) }, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		if (with_depth_stencil) {
			fb.depth_stencil = factory.createTexture({ uint16_t(w), uint16_t(h) }, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT);

			bgfx::Attachment attachments[2]{};
			attachments[0].init(fb.color->getTextureHandle());
			attachments[0].init(fb.depth_stencil->getTextureHandle());
			fb.fb = factory.createFrameBuffer(2, attachments, false);
		}
		else {
			bgfx::Attachment att;
			att.init(fb.color->getTextureHandle());
			fb.fb = factory.createFrameBuffer(1, &att, false);
		}

		return fb;
	}
	void RenderInterface_bgfx::destroyFrameBuffer(BgfxFrameBuffer& fb) {
		fb.fb.reset();
		fb.color.reset();
		fb.depth_stencil.reset();
		fb.fb = nullptr;
		fb.color = nullptr;
		fb.depth_stencil = nullptr;
	}


	//
	// Layer Stack
	//

	RenderInterface_bgfx::RenderLayerStack::~RenderLayerStack() {
		
	}
	void RenderInterface_bgfx::RenderLayerStack::destroyAll(RenderInterface_bgfx& ri) {
		for (auto& fb : fb_layers_) {
			ri.destroyFrameBuffer(fb);
		}
		fb_layers_.clear();
		for (auto& fb : fb_postprocess_) {
			ri.destroyFrameBuffer(fb);
		}
		fb_postprocess_.clear();

		layers_size_ = 0;
	}
	
	void RenderInterface_bgfx::RenderLayerStack::beginFrame(RenderInterface_bgfx& ri, int w, int h) {
		ri_ = &ri;
		if (w != width_ || h != height_) {
			RenderLayerStack::destroyAll(ri);
			width_ = w;
			height_ = h;
		}

		// === CRITICAL FIX: Pre-create ALL post-process buffers so vector never reallocates ===
		// This prevents dangling references mid-frame (the real cause of "garbage data")
		fb_postprocess_.reserve(8);
		while (fb_postprocess_.size() < 4) {
			this->fb_postprocess_.emplace_back(ri.createFrameBuffer(width_, height_, false));
		}

		layers_size_ = 0;
		RenderLayerStack::pushLayer(ri);
	}
	void RenderInterface_bgfx::RenderLayerStack::endFrame() {
		// Keep framebuffers alive for reuse next frame.
		// Just reset the active layer count.
		layers_size_ = 0;
	}

	Rml::LayerHandle RenderInterface_bgfx::RenderLayerStack::pushLayer(RenderInterface_bgfx& ri) {
		if (layers_size_ >= (int)fb_layers_.size()) {
			fb_layers_.push_back(ri.createFrameBuffer(width_, height_, true));
		}

		Rml::LayerHandle handle = static_cast<Rml::LayerHandle>(layers_size_);
		layers_size_++;
		return handle;
	}
	void RenderInterface_bgfx::RenderLayerStack::popLayer() {
		if (layers_size_ > 1)
			layers_size_--;
	}
	const BgfxFrameBuffer& RenderInterface_bgfx::RenderLayerStack::getLayer(Rml::LayerHandle h) const {
		return fb_layers_[static_cast<int>(h)];
	}

	const BgfxFrameBuffer& RenderInterface_bgfx::RenderLayerStack::getTopLayer() const {
		return fb_layers_[layers_size_ - 1];
	}
}
