#pragma once
#include "RenderContext.hpp"
#include "bgfx/bgfx.h"
#include <EASTL/unordered_map.h>

namespace mce::gfx {
	using core::QEventBus;
	class BgfxRenderContext final : public RenderContext {
	public:
		BgfxRenderContext(QEventBus& qBus);
		~BgfxRenderContext() override = default;

		bool init(sf::WindowBase& mainWindow, API api) override;
		void shutdown() override;

		void beginFrame() override;
		void endFrame() override;

		unsigned short registerWindow(sf::WindowBase& window) override;
		void unregisterWindow(uint16_t viewId) override;

		void resize(uint16_t viewId, uint32_t width, uint32_t height) override;

		bgfx::FrameBufferHandle getFrameBufferHandle(uint16_t viewId) override;
	private:
		struct WindowData {
			void* nativeHandle = nullptr;
			bgfx::FrameBufferHandle fbh = BGFX_INVALID_HANDLE;
			unsigned int width = 0;
			unsigned int height = 0;
			bool isMain = false;
		};

		uint16_t nextViewId = 1;

		eastl::unordered_map<uint16_t, WindowData> windows;

		void* getNativeHandle(sf::WindowBase& window);
	};
}