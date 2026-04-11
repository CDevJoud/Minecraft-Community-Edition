#include "Minecraft.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"
#include <filesystem>

#include "Graphics/BgfxRenderContext.hpp"
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>   // Added: ensure sf::Event is a complete type where used
#include <optional>               // Added: std::optional is used by translateEventAndDispatch
#include <Graphics/RenderFactory.hpp>

#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable:4996)
#endif

#define LOG_INFO(msg) qBus.post(event::Log(event::Log::INFO, msg))
#define LOG_ERROR(msg) qBus.post(event::Log(event::Log::ERROR, msg))

namespace mce {
	using core::QEventBus;
	using gfx::RenderContext;
	using gfx::RenderFactory;
	using io::VirtualFileSystem;

	Minecraft::Minecraft(const eastl::string_view& profileName, QEventBus& qBus, uint16_t viewId, sf::WindowHandle window, sf::Vector2u viewSize, eastl::shared_ptr<RenderContext>& renderCtx, RenderFactory& factory, VirtualFileSystem& vfs) :
		qBus(qBus),
		profileName(profileName.data(), profileName.size()),
		window(window),
		bIsRunning(false),
		renderCtx(renderCtx),
		viewSize(viewSize),
		viewId(viewId),
		renderer(viewId, factory, renderCtx->getRenderAPI()),
		vfs(vfs) {
		this->onClose = this->qBus.subscribeRAII<event::window::Close>([this](const event::window::Close& e) {
			if (e.window == this->window) {
				Minecraft::bIsRunning = false;
			}
			});
		this->onResize = this->qBus.subscribeRAII<event::window::Resize>([this](const event::window::Resize& e) {
			if (e.window == this->window) {
				this->viewSize.x = e.newSize.x;
				this->viewSize.y = e.newSize.y;
				this->renderCtx->resize(this->viewId, this->viewSize.x, this->viewSize.y);
			}
			});
		/*gfx::flags::Buffer bFlag;

		bFlag.addFlag(gfx::flags::Buffer::None);

		bool isSuccess = false;

		gfx::VertexBuffer vb({}, bFlag, isSuccess);

		*/

		
	}

	Minecraft::~Minecraft() {

		//renderCtx->shutdown();

		//MCE_INFO("Shutting down");
		//this->onClose;
		
	}

	int Minecraft::initInstance() {


		LOG_INFO(std::format("Init MCE: {}", profileName));

		renderer.init(vfs);

		/*
		* in the future here would we load the game assets from the vfs
		*/

		Minecraft::bIsRunning = true;

		LOG_INFO("Done");
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
		renderer.render();
	}
}
