#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>

#include <Graphics/Renderer.hpp>
#include <Graphics/Color.hpp>
#include <IO/VirtualFileSystem.hpp>

#include <EASTL/unordered_map.h>
#include <EASTL/fixed_vector.h>

#include <Core/ResourceManager.hpp>

namespace dms::ui::priv {
	class RmlVertex {
	public:
		RmlVertex();
		RmlVertex(sf::Vector2f position, gfx::Color color, sf::Vector2f texCoords);
		sf::Vector2f position;
		gfx::Color color;
		sf::Vector2f texCoords;

		static bgfx::VertexLayout layout();
	};
	enum class RmlProgramId : int {
		None = 0,
		Color,           // vertex-color only
		Texture,         // textured + vertex-color (premultiplied alpha)
		Passthrough,     // fullscreen blit / layer composite
		BlendMask,       // composite with blend-mask texture
		Blur,            // separable Gaussian blur
		DropShadow,      // drop-shadow filter
		ColorMatrix,     // generic 4×5 colour-matrix filter
		Creation,        // stencil-write (clip-mask geometry)
		Gradient,        // linear / radial / conic gradient shader
		Count
	};

	enum class FilterType { None, Blur, DropShadow, ColorMatrix, MaskImage };

	struct CompiledFilter {
		FilterType type = FilterType::None;
		// Blur / DropShadow
		float sigma = 0.f;
		// DropShadow extras
		Rml::Vector2f offset = { 0, 0 };
		Rml::Colourf  color = { 0, 0, 0, 1 };
		// ColorMatrix (row-major 4×5, stored as mat4 + vec4 translate)
		float color_matrix[16] = {};
		float color_translate[4] = {};
		// MaskImage
		eastl::shared_ptr<gfx::Texture> mask_texture = nullptr;
	};

	enum class ShaderType { None, Gradient };

	struct ShaderGradientStop { float position; Rml::Colourf color; };

	struct CompiledShaderData {
		ShaderType type = ShaderType::None;
		// Gradient
		int   gradient_function = 0; // 0=linear, 1=radial, 2=conic, 3=repeating-linear, …
		Rml::Vector2f p = {};
		Rml::Vector2f q = {};
		eastl::vector<ShaderGradientStop> stops;
		eastl::shared_ptr<gfx::Texture> stop_texture = nullptr;
	};

	struct BgfxFrameBuffer {
		eastl::shared_ptr<gfx::FrameBuffer> fb = nullptr;
		eastl::shared_ptr<gfx::Texture> color = nullptr;
		eastl::shared_ptr<gfx::Texture> depth_stencil = nullptr;
		int width = 0;
		int height = 0;
	};

	struct BgfxCompiledGeometry {
		eastl::shared_ptr<gfx::VertexBuffer> vb = nullptr;
		eastl::shared_ptr<gfx::IndexBuffer> ib = nullptr;
		int num_indices = 0;
	};

	class RenderInterface_bgfx : public Rml::RenderInterface, public gfx::Renderer {
	public:
		RenderInterface_bgfx(core::ResourceManager& rm);
		~RenderInterface_bgfx();

		void setViewport(int viewport_width, int viewport_height,
			int viewport_offset_x = 0, int viewport_offset_y = 0);

		void beginFrame(uint16_t viewId);
		void endFrame();

		Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
			Rml::Span<const int> indices) override;
		void RenderGeometry(Rml::CompiledGeometryHandle handle,
			Rml::Vector2f translation,
			Rml::TextureHandle texture) override;
		void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override;

		Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions,
			const Rml::String& source) override;
		Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source_data,
			Rml::Vector2i source_dimensions) override;
		void ReleaseTexture(Rml::TextureHandle texture_handle) override;

		void EnableScissorRegion(bool enable) override;
		void SetScissorRegion(Rml::Rectanglei region) override;

		void EnableClipMask(bool enable) override;
		void RenderToClipMask(Rml::ClipMaskOperation mask_operation,
			Rml::CompiledGeometryHandle geometry,
			Rml::Vector2f translation) override;

		void SetTransform(const Rml::Matrix4f* transform) override;

		Rml::LayerHandle PushLayer() override;
		void CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination,
			Rml::BlendMode blend_mode,
			Rml::Span<const Rml::CompiledFilterHandle> filters) override;
		void PopLayer() override;

		Rml::TextureHandle SaveLayerAsTexture() override;

		Rml::CompiledFilterHandle SaveLayerAsMaskImage() override;

		Rml::CompiledFilterHandle CompileFilter(const Rml::String& name,
			const Rml::Dictionary& parameters) override;
		void ReleaseFilter(Rml::CompiledFilterHandle filter) override;

		Rml::CompiledShaderHandle CompileShader(const Rml::String& name,
			const Rml::Dictionary& parameters) override;
		void RenderShader(Rml::CompiledShaderHandle shader_handle,
			Rml::CompiledGeometryHandle geometry_handle,
			Rml::Vector2f translation,
			Rml::TextureHandle texture) override;
		void ReleaseShader(Rml::CompiledShaderHandle effect_handle) override;

		// Special texture-handle sentinels (same semantics as GL3)
		static constexpr Rml::TextureHandle TextureEnableWithoutBinding = Rml::TextureHandle(-1);
		static constexpr Rml::TextureHandle TexturePostprocess = Rml::TextureHandle(-2);

	private:
		Rml::Matrix4f makeProjection() const;

		bool loadPrograms();
		void destroyPrograms();

		bgfx::ViewId allocateView();
		void setupView(bgfx::ViewId view, const BgfxFrameBuffer& fb);
		void setupViewSpace(bgfx::ViewId view);
		void ensureView();

		void useProgram(RmlProgramId id);

		void submitGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture, RmlProgramId program_override = RmlProgramId::None);
		void drawFullscreenQuad(const eastl::shared_ptr<gfx::Texture>& texture, RmlProgramId program);
		void drawFullscreenQuad(const eastl::shared_ptr<gfx::Texture>& texture, RmlProgramId program, Rml::Vector2f uv_offset, Rml::Vector2f uv_scaling, bool flip_v);
		void blitLayerToPostprocessPrimary(Rml::LayerHandle layerHandle);
		void renderFilters(Rml::Span<const Rml::CompiledFilterHandle> filterHandles);
		void renderBlur(float sigma, BgfxFrameBuffer& sourceDest, BgfxFrameBuffer& temp, Rml::Rectanglei windowFlipped);

		gfx::flags::State buildBaseState() const;
		uint32_t buildStencilState() const;
		
		void setTransformUniform(Rml::Vector2f translation);

		// FrameBuffer helpers
		BgfxFrameBuffer createFrameBuffer(int w, int h, bool with_depth_stencil = true);
		void destroyFrameBuffer(BgfxFrameBuffer& fb);

		bool frameBufferOriginBottomLeft = true;
	private:
		class RenderLayerStack {
		public:
			RenderLayerStack() = default;
			~RenderLayerStack();

			Rml::LayerHandle pushLayer(RenderInterface_bgfx& ri);
			void popLayer();

			const BgfxFrameBuffer& getLayer(Rml::LayerHandle h) const;
			const BgfxFrameBuffer& getTopLayer() const;
			Rml::LayerHandle getTopLayerHandle() const;

			BgfxFrameBuffer& getPostProcessPrimary();
			BgfxFrameBuffer& getPostProcessSecondary();
			BgfxFrameBuffer& getPostProcessTertiary();
			BgfxFrameBuffer& getBlendMask();

			void swapPostProcessPrimarySecondary();

			void beginFrame(RenderInterface_bgfx& ri, int w, int h);
			void endFrame();

		private:
			void destroyAll(RenderInterface_bgfx& ri);
			BgfxFrameBuffer& ensurePostProcess(RenderInterface_bgfx& ri, int idx);

			int width_ = 0;
			int height_ = 0;
			int layers_size_ = 0;

			eastl::vector<BgfxFrameBuffer> fb_layers_;
			eastl::vector<BgfxFrameBuffer> fb_postprocess_;

			RenderInterface_bgfx* ri_ = nullptr;
		};

		RenderLayerStack layers;

		// Programs
		eastl::fixed_vector<eastl::shared_ptr<gfx::ShaderProgram>, static_cast<int>(RmlProgramId::Count)> programs;
		RmlProgramId activeProgram;

		//// Uniforms (created once, reused)
		//bgfx::UniformHandle u_transform = BGFX_INVALID_HANDLE; // mat4
		//bgfx::UniformHandle u_translate = BGFX_INVALID_HANDLE; // vec4
		//bgfx::UniformHandle u_texParams = BGFX_INVALID_HANDLE; // vec4 (uv_offset.xy, uv_scale.xy)
		//bgfx::UniformHandle u_blurParams = BGFX_INVALID_HANDLE; // vec4 (sigma, dir_x, dir_y, 0)
		//bgfx::UniformHandle u_texelSize = BGFX_INVALID_HANDLE; // vec4 (1/w, 1/h, 0, 0)
		//bgfx::UniformHandle u_colorMatrix = BGFX_INVALID_HANDLE; // mat4
		//bgfx::UniformHandle u_colorTranslate = BGFX_INVALID_HANDLE; // vec4
		//bgfx::UniformHandle u_shadowExtra = BGFX_INVALID_HANDLE; // vec4 (offset.xy, 0, 0)
		//bgfx::UniformHandle u_shadowColor = BGFX_INVALID_HANDLE; // vec4
		//bgfx::UniformHandle u_gradientParams = BGFX_INVALID_HANDLE; // vec4 (func, num_stops, 0, 0)
		//bgfx::UniformHandle u_gradientP = BGFX_INVALID_HANDLE; // vec4 (p.xy, q.xy)
		//bgfx::UniformHandle s_texture0 = BGFX_INVALID_HANDLE; // sampler
		//bgfx::UniformHandle s_texture1 = BGFX_INVALID_HANDLE; // sampler (blend mask / stops)
		eastl::shared_ptr<gfx::ShaderProgram> shader; // we load all the uniforms manually when calling `shader->setUniform()`

		// Geometry registries
		Rml::CompiledGeometryHandle nextGeometryId;
		eastl::unordered_map<Rml::CompiledGeometryHandle, BgfxCompiledGeometry> geometries;

		// Filter / Shader registries
		Rml::CompiledFilterHandle nextFilterId;
		eastl::unordered_map<Rml::CompiledFilterHandle, CompiledFilter> filters;

		Rml::CompiledShaderHandle nextShaderId;
		eastl::unordered_map<Rml::CompiledShaderHandle, CompiledShaderData> shaders;

		// Texture bookkeeping
		Rml::TextureHandle nextTextureId;

		//Fullscreen-quad geometry (pre-built)
		Rml::CompiledGeometryHandle fullscreenQuad = 0;

		// Render state
		Rml::Matrix4f transform;
		Rml::Matrix4f projection;

		bool scissorEnabled;
		Rml::Rectanglei scissorRegion;

		bool clipMaskEnabled;
		uint8_t stencilRef; 
		uint8_t stencilValue; 

		uint16_t currentViewId;
		uint16_t baseView; 
		bool viewDirty;

		//bgfx::TextureHandle lastBoundTexture = BGFX_INVALID_HANDLE;
		eastl::shared_ptr<gfx::Texture> lastBoundTexture;

		int viewportWidth;
		int viewportHeight;
		int viewportOffsetX;
		int viewportOffsetY;

		bool valid;

		uint32_t drawOrder;
		core::ResourceManager& rsrcMgr; // resource manager

		eastl::vector<eastl::shared_ptr<gfx::Texture>> generatedTextures;
	};
}

