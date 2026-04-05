#include "Minecraft.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"
#include <filesystem>

#include "Graphics/BgfxRenderContext.hpp"
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>   // Added: ensure sf::Event is a complete type where used
#include <optional>               // Added: std::optional is used by translateEventAndDispatch

#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable:4996)
#endif

namespace mce {
	Minecraft::Minecraft(const eastl::string_view& profileName, QEventBus& qBus, uint16_t viewId, sf::WindowHandle window, sf::Vector2u viewSize, eastl::shared_ptr<RenderContext>& renderCtx) :
		qBus(qBus),
		profileName(profileName.data()),
		window(window),
		bIsRunning(false),
		renderCtx(renderCtx),
		viewSize(viewSize),
		viewId(viewId) {
		this->onClose = this->qBus.subscribeRAII<event::window::Close>([this](const event::window::Close& e) {
			if (e.window == this->window) {
				Minecraft::bIsRunning = false;
				this->renderCtx->unregisterWindow(this->viewId);
			}
			});
		this->onResize = this->qBus.subscribeRAII<event::window::Resize>([this](const event::window::Resize& e) {
			if (e.window == this->window) {
				this->viewSize.x = e.newSize.x;
				this->viewSize.y = e.newSize.y;
				this->renderCtx->resize(this->viewId, this->viewSize.x, this->viewSize.y);
			}
			});
	}

	Minecraft::~Minecraft() {

		//renderCtx->shutdown();

		//MCE_INFO("Shutting down");
		//this->onClose;
	}

	int Minecraft::initInstance() {


		MCE_INFO("Init MCE: {}", profileName);

		/*
		* in the future here would we load the game assets from the vfs
		*/

		Minecraft::bIsRunning = true;

		MCE_INFO("Done");
		return 0;
	}

	int Minecraft::run() {

		int retCode = Minecraft::initInstance();

		if (retCode < 0) {
			return retCode;
		}

		while (Minecraft::bIsRunning) {

			//there is nothing to do so we sleep so we wont let the thread loop so quickly
			sf::sleep(sf::milliseconds(10));
		}
		return 0;
	}

	void Minecraft::render() {
		bgfx::touch(this->viewId);
	}
}
