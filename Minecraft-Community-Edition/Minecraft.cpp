#include "Minecraft.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"
#include <filesystem>

#include "SFML/Window/VideoMode.hpp"

#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable:4996)
#endif

namespace mce {
	Minecraft::Minecraft(const eastl::string_view& profileName) :  
		qBus(profileName.data()) {
		qBus.runAsync();
	}

	Minecraft::~Minecraft() {
		//MCE_INFO("Shutting down");
	}

	int Minecraft::initInstance() {

		//Minecraft::setupLogging();

		MCE_INFO("Starting MCE");
		window.create(sf::VideoMode({ 1280, 720 }), "Minecraft: Community Edition");
		graphicsContext = GraphicsContext::create(GraphicsContext::API::OpenGL);
		renderer = graphicsContext->createRenderer(window);
		MCE_INFO("Done");

		return 0;
	}

	void Minecraft::handleEvents() {
		while (const auto event = window.pollEvent()) {

		}
	}

	inline void Minecraft::translateEventAndDispatch(const std::optional<sf::Event> event) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		auto key = event->getIf<sf::Event::KeyPressed>();
		key->code == sf::Keyboard::Key::A;
		
	}

	int Minecraft::run() {
		
		Minecraft::initInstance();

		sf::Clock clock;
		int FramesCounter = 0;
		clock.restart();
		while (window.isOpen()) {
			while (const auto event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>()) {
					window.close();
					return 1;
				}
			}
			sf::Event;
			sf::Time dt = clock.getElapsedTime();
			if (dt >= sf::seconds(1.0f)) {
				window.setTitle("FPS: " + std::to_string(FramesCounter));
				FramesCounter = 0;
				clock.restart();
			}
			else {
				++FramesCounter;
			}

			renderer->renderFrame();
			window.display();
		}
		return 0;
	}
	
	std::string Minecraft::getLogFileName() {
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* pTime = std::localtime(&currentTime);

		// strftime adds the null terminator, so initializing isn't strictly necessary
		char filename[32];
		std::strftime(filename, sizeof(filename), "logs/%Y-%m-%d %H-%M-%S.txt", pTime);

		return filename;
	}
	void Minecraft::setupLogging() {
		namespace fs = std::filesystem;

		if (!fs::is_directory("logs"))
			fs::create_directory("logs");

		eastl::shared_ptr<mce::io::FileSink> fileSink = eastl::make_shared<mce::io::FileSink>(getLogFileName());

		mce::io::Logger& logger = mce::io::Logger::getGlobalLogger(qBus);
		logger.addSink(fileSink);
	}
}
