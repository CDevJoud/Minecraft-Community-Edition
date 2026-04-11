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
#include <bx/os.h>
#include <SFML/Graphics/Image.hpp>
#include "TUI/icon.hpp"
#include "TUI/CLogger.hpp"

sf::Image iImg;


#define LOG_DEBUG(msg) qBus.post(event::Log(event::Log::Severity::DEBUG, msg));
namespace mce {
	Application::Application(int argc, char* argv[]) :
		qBus("APP"),
		threadManager(qBus),
		cmd(argc, argv),
		factory(qBus),
		fLogger(qBus),
		console(qBus, nullptr, "Minecraft Community Edition Debugger", sf::VideoMode::getDesktopMode().width / 8, sf::VideoMode::getDesktopMode().height / 16, 8, 16) {
		setupLogging();
		qBus.runAsync();
		iImg.loadFromMemory(icon, icon_size);


		console.insertComponent(tui::CLogger::createInstance(qBus, "bgfx", 117, 30));
		console.insertComponent(tui::CLogger::createInstance(qBus, "default", 115, 30));

		auto component = console.getComponent<tui::CLogger>("default");
		component->setPosition(119, 1);
		Application::isApplicationInit = Application::initApplication();
		qBus.post(event::Log(event::Log::Severity::DEBUG, "Hello World!"));
		LOG_DEBUG("Hello World!");
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
				
				sf::sleep(sf::milliseconds(0));
			}
		}
		console.close();
		threadManager.waitAll();

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
		/*if (!vfs.loadFile("assets")) {
			vfs.buildJSONMappingFile("assets.json", "assets.bin");
			vfs.loadFile("assets");
		}*/

		Application::initQEventBusSubscription();

		renderCtx = eastl::make_shared<gfx::BgfxRenderContext>(qBus);

		Application::createProfile("MCE:Player1");

		return true;
	}

	void Application::initQEventBusSubscription() {

	}
	void Application::createProfile(const eastl::string profileName) {
		eastl::unique_ptr<sf::WindowBase> window = eastl::make_unique<sf::WindowBase>(sf::VideoMode(1920, 1080), "Minecraft CE");
		window->setIcon(iImg.getSize().x, iImg.getSize().y, iImg.getPixelsPtr());
		uint16_t viewId = 0;
		if (!this->isRenderCtxInit) {
			if (renderCtx->init(*window, api)) {
				this->isRenderCtxInit = true;
			}
		}
		else {
			viewId = renderCtx->registerWindow(*window);
		}
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

		Application::instances.emplace_back(
			eastl::make_pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>(eastl::move(window), eastl::move(mc))
		);


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

#include <Windows.h>
MCE_STARTUP(mce::Application);


////Trying to find the error LMAO
//int main() {
//	sf::WindowBase window(sf::VideoMode(1280, 720), "");
//	mce::core::QEventBus qBus("APP");
//	qBus.runAsync();
//	mce::gfx::BgfxRenderContext ctx(qBus);
//
//	mce::io::VirtualFileSystem vfs;
//	vfs.loadFile("assets");
//
//	ctx.init(window, mce::gfx::RenderContext::API::Direct3D11);
//	bgfx::setDebug(BGFX_DEBUG_STATS);
//
//	mce::gfx::RenderFactory factory(qBus);
//	
//	mce::gfx::VertexArray vArray;
//	
//	vArray.append(mce::gfx::Vertex(sf::Vector3f(1.0f, 1.0f, 1.0f), sf::Color::Red, sf::Vector2f(1.0f, 1.0f)));
//	auto layout = mce::gfx::Vertex::layout();
//	
//	vArray.setVertexLayout(layout);
//	mce::gfx::flags::Buffer bFlag;
//	
//	{
//		bFlag.addFlag(mce::gfx::flags::Buffer::None);
//		auto vb = factory.createVertexBuffer(vArray, bFlag, "VertexBuffer");
//
//		eastl::vector<uint8_t> vsBytes, fsBytes;
//		vfs.getFile("assets.shaders.main.vs.d3d11_windows", vsBytes);
//
//		vfs.getFile("assets.shaders.main.fs.d3d11_windows", fsBytes);
//
//		auto sp = factory.createShaderProgram(eastl::make_pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>(vsBytes, fsBytes));
//
//		bool success = false;
//		//auto sp = mce::gfx::ShaderProgram(vsBytes, fsBytes, success);
//
//		/*const bgfx::Memory* vsMem = bgfx::makeRef(vsBytes.data(), vsBytes.size());
//		const bgfx::Memory* fsMem = bgfx::makeRef(fsBytes.data(), fsBytes.size());
//
//		bgfx::ShaderHandle vs = bgfx::createShader(vsMem);
//		bgfx::ShaderHandle fs = bgfx::createShader(fsMem);
//
//		bgfx::ProgramHandle program = bgfx::createProgram(vs, fs, true);*/
//
//		while (window.isOpen()) {
//			for (sf::Event event; window.pollEvent(event);) {
//				if (event.type == sf::Event::Closed) {
//					window.close();
//				}
//			}
//
//			ctx.beginFrame();
//
//			ctx.endFrame();
//		}
//		//bgfx::destroy(program);
//	}
//
//	ctx.shutdown();
//}
