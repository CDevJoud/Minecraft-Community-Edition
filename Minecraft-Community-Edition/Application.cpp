#pragma warning(disable:4996)
#include "Startup.hpp"

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Event.hpp"

#include "Application.hpp"
#include "Graphics/BgfxRenderContext.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Clock.hpp>
#include <filesystem>
#include <chrono>
#include <bx/commandline.h>

namespace mce {
	Application::Application(int argc, char* argv[]) :
		qBus("APP"),
		threadManager(qBus),
		cmd(argc, argv) {
		Application::isApplicationInit = Application::initApplication();
	}

	int Application::run() {
		if (!Application::isApplicationInit)
			return -1;
		sf::Clock deltaClock;
		sf::Clock fpsClock;

		int frameCount = 0;
		int currentFPS = 0;
		bgfx::setDebug(BGFX_DEBUG_STATS);
		while (true) {
			{
				std::lock_guard<std::mutex> lock(instanceMutex);

				if (instances.empty()) {
					break;
				}

				sf::Time dt = deltaClock.restart(); // frame time (if you need it)

				frameCount++;

				if (fpsClock.getElapsedTime() >= sf::seconds(1.0f)) {
					currentFPS = frameCount;
					frameCount = 0;
					fpsClock.restart();

					for (auto& instance : Application::instances) {
						instance.first->setTitle("FPS: " + std::to_string(currentFPS));
					}
				}

				for (auto& instance : Application::instances) {
					for (sf::Event event{}; instance.first->pollEvent(event);) {
						if (event.type == sf::Event::Closed) {
							qBus.post(event::window::Close{ instance.first->getSystemHandle() });
							Application::renderCtx->unregisterWindow(instance.second->getViewId());
							instance.first->close();
						}
						if (event.type == sf::Event::Resized) {
							qBus.post(event::window::Resize{ instance.first->getSystemHandle(), { event.size.width, event.size.height } });
						}
					}
				}

				for (auto& instance : Application::instances) {
					if (instance.second && instance.second->isRunning()) {
						instance.second->render();
					}
				}
				bgfx::frame();

			}
			//qBus.post(event::BeginFrame{});
			qBus.process();

			sf::sleep(sf::milliseconds(0));
		}

		renderCtx->shutdown();

		//process events before exiting
		sf::sleep(sf::milliseconds(100));
		while (qBus.getQueueSize() > 0) {
			qBus.process();
		}
		return 0;
	}
	
	std::string Application::getLogFileName() {
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* pTime = std::localtime(&currentTime);

		// strftime adds the null terminator, so initializing isn't strictly necessary
		char filename[32];
		std::strftime(filename, sizeof(filename), "logs/%Y-%m-%d %H-%M-%S.txt", pTime);

		return filename;
	}
	void Application::setupLogging() {
		namespace fs = std::filesystem;

		if (!fs::is_directory("logs"))
			fs::create_directory("logs");

		eastl::shared_ptr<mce::io::FileSink> fileSink = eastl::make_shared<mce::io::FileSink>(getLogFileName());

		mce::io::Logger& logger = mce::io::Logger::getGlobalLogger(qBus);
		logger.addSink(fileSink);
	}

	bool Application::initApplication() {

		std::string renderer = cmd.findOption('\0', "renderer");
		if (renderer == "d3d12") {
			api = gfx::RenderContext::API::Direct3D12;
		}
		else if (renderer == "gl") {
			api = gfx::RenderContext::API::OpenGL;
		}
		else if (renderer == "d3d11") {
			api = gfx::RenderContext::API::Direct3D11;
		}
		else if (renderer == "vulkan") {
			api = gfx::RenderContext::API::Vulkan;
		}
		else {
			api = gfx::RenderContext::API::Count;
		}
		setupLogging();

		Application::initQEventBusSubscription();

		renderCtx = eastl::make_shared<gfx::BgfxRenderContext>(qBus);

		Application::createProfile("MCE:Player1");
		return true;
	}

	void Application::initQEventBusSubscription() {
		
	}
	void Application::createProfile(const eastl::string profileName) {
		eastl::unique_ptr<sf::WindowBase> window = eastl::make_unique<sf::WindowBase>(sf::VideoMode(1280, 720), "Minecraft CE");

		uint16_t viewId = 0;
		if(!this->isRenderCtxInit) {
			if (renderCtx->init(*window, api)) {
				this->isRenderCtxInit = true;
			}
		}
		else {
			viewId = renderCtx->registerWindow(*window);
		}
		//For now we will have a single profile
		//Application::instances.emplace_back(eastl::make_unique<Minecraft>(profileName, qBus, viewId, window->getSystemHandle(), window->getSize().x, window->getSize().y, renderCtx));

		eastl::unique_ptr<Minecraft> mc = eastl::make_unique<Minecraft>(
			profileName,
			qBus,
			viewId,
			window->getSystemHandle(),
			window->getSize(),
			Application::renderCtx
		);

		Minecraft* rawMinecraftPtr = mc.get();

		Application::instances.emplace_back(
			eastl::make_pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>(eastl::move(window), eastl::move(mc))
		);

		//Minecraft* instance = instances.back().second.get();

		//create a new thread so we dont block the main thread
		threadManager.createThread(profileName, [this, rawMinecraftPtr]() {
			rawMinecraftPtr->run();

			std::lock_guard<std::mutex> lock(instanceMutex);

			auto it = eastl::find_if(instances.begin(), instances.end(), [rawMinecraftPtr](const eastl::pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>& m) {
				return m.second.get() == rawMinecraftPtr;
				});

			if (it != instances.end()) {
				instances.erase(it);
			}

		})->launch();
	}
}

MCE_STARTUP(mce::Application);