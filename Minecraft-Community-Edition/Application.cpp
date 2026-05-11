#pragma warning(disable:4996)
#include "Startup.hpp"

#include "Core/VideoMode.hpp"
#include "Core/Event.hpp"

#include "Application.hpp"
#include "Graphics/BgfxRenderContext.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"

#include "Core/Sleep.hpp"
#include "Core/Clock.hpp"

#include <filesystem>
#include <chrono>

#include "libs/bx/commandline.h"

#include <Graphics/Image.hpp>
#include "TUI/icon.hpp"
#include "TUI/CLogger.hpp"
#include "libs/bx/os.h"

#include <RmlUi/Core.h>
#include <UI/Backend/RmlUI_Platform_dms.hpp>
#include <UI/Backend/RmlUI_Renderer_bgfx.hpp>
#include <RmlUi/Core/FontEngineInterface.h>

mce::gfx::Image iImg;
#include "Mod/ModLoader.h"

typedef Xconst XAPIDescriptor(XAPI_STDCALL* XI_queryFn)(Xvoid);
typedef Xconst Xint32(XAPI_STDCALL* XI_mainFn)(Xvoid Xconstptr);
typedef Xconst Xint32(XAPI_STDCALL* XI_terminateFn)(Xvoid Xconstptr);

XI_queryFn XI_query;
XI_mainFn XI_main;
XI_terminateFn XI_terminate;
XIExports exports;

#define LOG_DEBUG(msg) qBus.post(event::Log(event::Log::Severity::DEBUG, msg));
namespace mce {
	Application::Application(int argc, char* argv[]) :
		qBus("APP"),
		threadManager(qBus),
		cmd(argc, argv),
		factory(qBus),
		fLogger(qBus),
		console(qBus, nullptr, "Minecraft Community Edition Debugger", sf::VideoMode::getDesktopMode().width / 8, sf::VideoMode::getDesktopMode().height / 16, 8, 16) {
		//just a funny way to pass data to methods/constructor. might be used in the future for modding for function parameters that are to long
		/*console({
			{"qBus", (uint64_t)(&qBus)},
			{"window", {
					{"nwh", (uint64_t)nullptr},
					{"title", "Minecraft Community Edition Debugger"},
					{"width", sf::VideoMode::getDesktopMode().width / 8},
					{"height", sf::VideoMode::getDesktopMode().height / 16},
					{"pxlWidth", 8},
					{"pxlHeight", 16},
					{"type", 1}
				}
			}
			})*/ 
		setupLogging();
		qBus.runAsync();
		iImg.loadFromMemory(icon, icon_size);
		vfs.buildJSONMappingFile("assets.json", "assets.bin");
		console.insertComponent(tui::CLogger::createInstance(qBus, "default", 115, 30));
		console.insertComponent(tui::CLogger::createInstance(qBus, "bgfx", 117, 30));
		auto component = console.getComponent<tui::CLogger>("default");
		component->setPosition(119, 1);
		Application::isApplicationInit = Application::initApplication();

		nlohmann::json data = { {"renderCtx", (uint64_t)(&renderCtx)} };
	
		LOG_DEBUG("Loading Sample Mod...");
		setGlobalQEventBus(&qBus);
		//loadMod("net9.0-windows10.0.26100.0\\win-x64\\publish\\SampleModC#.dll");
		loadMod("SampleMod.dll");

		LOG_DEBUG("Sample Mod loaded successfuly!");
	}

	int Application::run() {
		if (!Application::isApplicationInit)
			return -1;
		sf::Clock deltaClock;
		sf::Clock fpsClock;

		int frameCount = 0;
		int currentFPS = 0;
		int isWindowsNotVisible = 40;
		bgfx::setDebug(BGFX_DEBUG_STATS);
		while (true) {
			{
				std::lock_guard<std::mutex> lock(instanceMutex);

				if (instances.empty()) {
					break;
				}
				if(exports.onUpdate)
					exports.onUpdate();
				sf::Time dt = deltaClock.restart(); 

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
					//std::unique_lock<std::mutex> lock(instanceMutex);
					for (sf::Event event{}; instance.first->pollEvent(event);) {
						if (event.type == sf::Event::Closed) {
							qBus.post(event::window::Close{ instance.first->getSystemHandle() });
							Application::renderCtx->unregisterWindow(instance.second->getViewId());
							instance.first->close();
						}
						if (event.type == sf::Event::Resized) {
							qBus.post(event::window::Resize{ instance.first->getSystemHandle(), { event.size.width, event.size.height } });
						}
						if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
							ctx->UnloadDocument(doc);
							doc = ctx->LoadDocument(".\\assets\\ui\\main.html");
							doc->Show();
						}
						ui::priv::inputEventHandler(ctx, instance.first->getSystemHandle(), event);
					}
				}
				Application::renderCtx->beginFrame();

				for (auto& instance : Application::instances) {
					if (instance.second && instance.second->isRunning()) {
						instance.second->render();
					}
				}
				ctx->Update();
				ctx->Render();

				Application::renderCtx->endFrame();

				//sf::sleep(sf::milliseconds(0));
			}
		}
		LOG_DEBUG("Unloading Sample Mod");
		exports.onShutdown();
		XI_terminate(mce_destroyDeviceAndContext);
		bx::dlclose(hMod);
		console.close();

		bool hasBgfxShutdown = false;
		Thread* th = threadManager.createThread("BgfxShutdown", [this, &hasBgfxShutdown]() {
			bgfx::frame(BGFX_FRAME_FLUSH | BGFX_FRAME_DISCARD);
			renderCtx->shutdown();
			hasBgfxShutdown = true;
			});

		th->launch();

		//Wait for 3 seconds if bgfx refuses to shutdown then we terminates it
		sf::sleep(sf::seconds(3));
		if(!hasBgfxShutdown)
			th->terminate();

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

		//eastl::shared_ptr<mce::io::FileSink> fileSink = eastl::make_shared<mce::io::FileSink>(getLogFileName());

		////mce::io::Logger& logger = mce::io::Logger::getGlobalLogger(qBus);
		//logger.addSink(fileSink);
		fLogger.open(getLogFileName());
	}

	bool Application::initApplication() {
		
		threadManager.createThread("ConsoleOutput", [&]() {
			while (console.isOpen()) {
				console.clear();

				console.display();
				sf::sleep(sf::milliseconds(10));
			}
			})->launch();

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

		vfs.loadFile("assets");

		Application::initQEventBusSubscription();

		Application::renderCtx = eastl::make_shared<gfx::BgfxRenderContext>(qBus);

		Application::rsrcMgr = eastl::make_unique<core::ResourceManager>(qBus, vfs, factory, *renderCtx);
		
		Application::appWindow = eastl::make_unique<sf::WindowBase>(sf::VideoMode::getDesktopMode(), "Damascene RunTime Engine: v0.2.0");

		if (!Application::isRenderCtxInit) {
			if (renderCtx->init(*appWindow, api)) {
				isRenderCtxInit = true;
			}
		}

		if (rmlSystem == nullptr)
			Application::rmlSystem = new ui::priv::SystemInterface_dms(qBus);
		if (rmlRenderer == nullptr)
			Application::rmlRenderer = new ui::priv::RenderInterface_bgfx(*rsrcMgr);

		Rml::SetSystemInterface(rmlSystem);
		Rml::SetRenderInterface(rmlRenderer);

		if (!Rml::Initialise()) {
			return false;
		}

		vfs.getFile("assets.fonts.arial_black", fontMem);
		Rml::Span<const Rml::byte> rmlFontMem(
			reinterpret_cast<const Rml::byte*>(fontMem.data()),
			fontMem.size()
		);
		
		Rml::LoadFontFace(rmlFontMem, "arial_black", Rml::Style::FontStyle::Normal);
		Application::ctx = Rml::CreateContext("main", Rml::Vector2i(1920, 1080));

		doc = Application::ctx->LoadDocument(".\\assets\\ui\\main.html");
		doc->Show();
		
		Application::createProfile("MCE:Player1", eastl::move(appWindow));
		
		return true;
	}

	void Application::initQEventBusSubscription() {
		
	}
	void Application::createProfile(const eastl::string profileName) {
		eastl::unique_ptr<sf::WindowBase> window = eastl::make_unique<sf::WindowBase>(sf::VideoMode(1920 / 4, 1080 / 2), "Minecraft CE");
		window->setIcon(iImg.getSize().x, iImg.getSize().y, iImg.getPixelsPtr());
		uint16_t viewId = 0;
		if (window == nullptr) {
			window = eastl::make_unique<sf::WindowBase>(sf::VideoMode(1920, 1080), "Minecraft CE");
			viewId = renderCtx->registerWindow(*window);
		}

		window->setIcon(iImg.getSize().x, iImg.getSize().y, iImg.getPixelsPtr());

		eastl::unique_ptr<Minecraft> mc = eastl::make_unique<Minecraft>(
			profileName,
			Application::qBus,
			viewId,
			window->getSystemHandle(),
			window->getSize(),
			Application::renderCtx,
			Application::factory,
			Application::vfs
		);

		Minecraft* rawMinecraftPtr = mc.get();
		sf::WindowBase* rawWindowPtr = window.get();
		Application::instances.emplace_back(
			eastl::make_pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>(eastl::move(window), eastl::move(mc))
		);

		rmlRenderer->setViewport(rawWindowPtr->getSize().x, rawWindowPtr->getSize().y, 0, 0);

		//create a new thread so we don't block the main thread
		threadManager.createThread(profileName, [this, rawMinecraftPtr]() {
			rawMinecraftPtr->run();

			std::lock_guard<std::mutex> lock(instanceMutex);
			
			auto it = eastl::find_if(instances.begin(), instances.end(), [rawMinecraftPtr](const eastl::pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>& m) {
				return m.second.get() == rawMinecraftPtr;
				});
			
			// we need the main window that init the bgfx to stay alive until we destroy bgfx
			// if this window is the master/main window then we pass it to appWindow.
			// this is a temp fix for now!
			if (it->first->bIsMaster) {
				Application::appWindow = eastl::move(it->first);
			}
			else {
				renderCtx->unregisterWindow(it->second->getViewId());
			}

			if (it != instances.end()) {
				instances.erase(it);
			}

			})->launch();
		rawWindowPtr->setVisible(true);

		
	}
}

#include <Windows.h>
MCE_STARTUP(mce::Application);