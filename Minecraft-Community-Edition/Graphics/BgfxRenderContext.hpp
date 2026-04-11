#pragma once
#include "RenderContext.hpp"
#define BGFX_CONFIG_MULTITHREADED 0
#include "bgfx/bgfx.h"
#include <EASTL/unordered_map.h>
#include "..\BgfxCallBack.hpp"

namespace mce::gfx {
	class BgfxRenderContext final : public RenderContext {
	public:
		BgfxRenderContext(core::QEventBus& qBus);
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

		BgfxCallBack* bgfxCallBack;

		uint16_t nextViewId = 1;

		eastl::unordered_map<uint16_t, WindowData> windows;

		void* getNativeHandle(sf::WindowBase& window);
	};
}