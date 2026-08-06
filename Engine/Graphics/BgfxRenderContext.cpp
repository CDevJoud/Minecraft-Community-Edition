#include "Platform.hpp"
#include "BgfxRenderContext.hpp"
#include <Core/WindowBase.hpp>
#include "IO/Logger.hpp"

using dms::core::QEventBus;

#define LOG_INFO(msg) qBus.post(event::Log(event::Log::INFO, msg, "bgfx"))
#define LOG_ERROR(msg) qBus.post(event::Log(event::Log::ERROR, msg, "bgfx"))

namespace dms::gfx {
	
	void* BgfxRenderContext::getNativeHandle(sf::WindowBase& window) {
#if defined(MCE_PLATFORM_WINDOWS)
		return (void*)window.getSystemHandle();
#else
		return (void*)window.getSystemHandle();
#endif
	}

	BgfxRenderContext::BgfxRenderContext(QEventBus& qBus) : RenderContext(qBus), bgfxCallBack(new BgfxCallBack(qBus)) {
		
	}

	bool BgfxRenderContext::init(sf::WindowBase& mainWindow, API api) {

		LOG_INFO(std::format("Init bgfx RenderContext with res:{}x{}, API:{}, GraphicDevice:NVIDIA",
			mainWindow.getSize().x, mainWindow.getSize().y,
			bgfx::getRendererName(bgfx::RendererType::Enum(api))));

		//disable Bgfx Multithreaded
		//bgfx::renderFrame();

		bgfx::Init init{};
		init.type = bgfx::RendererType::Enum(api);
		init.platformData.nwh = BgfxRenderContext::getNativeHandle(mainWindow);
		init.resolution.width = mainWindow.getSize().x;
		init.resolution.height = mainWindow.getSize().y;
		init.resolution.reset = BGFX_RESET_NONE;
		init.vendorId = BGFX_PCI_ID_NVIDIA;
		init.callback = bgfxCallBack;

		if (!bgfx::init(init)) {
			LOG_ERROR(std::format(
				"Could not init bgfx RenderContext for: {}",
				bgfx::getRendererName(init.type)
			));
			return false;
		}

		WindowData data{};
		data.nativeHandle = init.platformData.nwh;
		data.width = mainWindow.getSize().x;
		data.height = mainWindow.getSize().y;
		data.isMain = true;

		mainWindow.bIsMaster = true;

		BgfxRenderContext::windows[0] = data;

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF);
		bgfx::setViewRect(0, 0, 0, data.width, data.height);

		LOG_INFO("bgfx RenderContext has successfully init! main window assigned to viewId of 0");

		backendAPI = api;
		return true;
	}
	
	void BgfxRenderContext::shutdown() {
		for (auto& [viewId, win] : BgfxRenderContext::windows) {
			if (!win.isMain && bgfx::isValid(win.fbh)) {
				bgfx::destroy(win.fbh);
			}
		}
		BgfxRenderContext::windows.clear();
		bgfx::shutdown();
		delete bgfxCallBack;
	}

	void BgfxRenderContext::beginFrame() {
		for (auto& [viewId, win] : BgfxRenderContext::windows) {
			bgfx::setViewRect(viewId, 0, 0, win.width, win.height);

			if (!win.isMain) {
				bgfx::setViewFrameBuffer(viewId, win.fbh);
			}
			bgfx::touch(viewId);
		}
	}

	void BgfxRenderContext::endFrame() {
		bgfx::frame();
	}


	uint16_t BgfxRenderContext::registerWindow(sf::WindowBase& window) {
		uint16_t viewId = BgfxRenderContext::nextViewId++;

		WindowData data{};
		data.nativeHandle = BgfxRenderContext::getNativeHandle(window);
		data.width = window.getSize().x;
		data.height = window.getSize().y;
		data.isMain = false;

		bgfx::PlatformData pd{};
		pd.nwh = data.nativeHandle;

		data.fbh = bgfx::createFrameBuffer(
			pd.nwh,
			data.width,
			data.height
		);

		if (!bgfx::isValid(data.fbh)) {
			LOG_ERROR("Could not create a frame buffer handle");
			return 0xFFFF;
		}

		BgfxRenderContext::windows[viewId] = data;
		bgfx::setViewFrameBuffer(viewId, data.fbh);
		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF);
		bgfx::setViewRect(viewId, 0, 0, data.width, data.height);
		return viewId;
	}

	void BgfxRenderContext::unregisterWindow(uint16_t viewId) {
		auto it = BgfxRenderContext::windows.find(viewId);
		if (it == BgfxRenderContext::windows.end()) {
			return;
		}

		if (!it->second.isMain && bgfx::isValid(it->second.fbh)) {
			bgfx::destroy(it->second.fbh);
		}

		BgfxRenderContext::windows.erase(it);
	}

	void BgfxRenderContext::resize(uint16_t viewId, uint32_t width, uint32_t height) {
		auto& win = BgfxRenderContext::windows[viewId];

		win.width = width;
		win.height = height;

		if (win.isMain) {
			bgfx::reset(width, height, BGFX_RESET_NONE);
			bgfx::setViewRect(0, 0, 0, width, height);
			return;
		}

		if (bgfx::isValid(win.fbh)) {
			bgfx::destroy(win.fbh);
		}

		bgfx::PlatformData pd{};
		pd.nwh = win.nativeHandle;

		win.fbh = bgfx::createFrameBuffer(
			pd.nwh,
			width,
			height
		);

		bgfx::setViewFrameBuffer(viewId, win.fbh);
		bgfx::setViewRect(viewId, 0, 0, width, height);
	}
	bgfx::FrameBufferHandle BgfxRenderContext::getFrameBufferHandle(uint16_t viewId) {
		auto it = BgfxRenderContext::windows.find(viewId);
		if (it == BgfxRenderContext::windows.end()) {
			return BGFX_INVALID_HANDLE;
		}
		else {
			return it->second.fbh;
		}
	}
	BgfxRenderContext::WindowData BgfxRenderContext::getWindowData(uint16_t viewId) {
		auto it = BgfxRenderContext::windows.find(viewId);
		if (it == BgfxRenderContext::windows.end()) {
			return BgfxRenderContext::WindowData{};
		}
		else {
			return it->second;
		}
	}
}
