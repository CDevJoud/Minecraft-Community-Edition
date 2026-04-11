#pragma once
#include "Minecraft.hpp"
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>
#include "Core/QEventBus.hpp"
#include "Core/ThreadManager.hpp"
#include "SFML/Window/Window.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderFactory.hpp"
#include "bx/commandline.h"
#include "IO/VirtualFileSystem.hpp"
#include "tui/Console.hpp"
#include "IO/FileLogger.hpp"

namespace mce {
	/**
 * @class Application
 * @brief Main application class managing multiple game instances.
 *
 * This class serves as the main core of the program. It manages
 * multiple instances of Minecraft, allowing the game to run
 * across multiple windows or monitors. Each instance can
 * represent a different player profile, providing multi-profile
 * gameplay without relying on splitscreen.
 *
 * @note Each instance is independent and can be configured separately.
 * @todo Implement instance management and window/multi-monitor support.
 */
	class Application : public Startup {
	public:
		/**
		 * @brief Default constructor.
		 *
		 * Initializes the application and prepares the instance manager.
		 */
		Application(int argc, char* argv[]);

		/**
		 * @brief Starts the application.
		 *
		 * Launches all managed game instances and handles the main loop.
		 */
		virtual int run() override;

	private:
		std::string getLogFileName();

		void setupLogging();

		bool initApplication();
		
	private:
		void initQEventBusSubscription();
		void createProfile(const eastl::string profileName);

		eastl::vector<eastl::pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>> instances;

		std::mutex instanceMutex;
		core::QEventBus qBus;
		core::ThreadManager threadManager;
		eastl::shared_ptr<gfx::RenderContext> renderCtx;
		bx::CommandLine cmd;
		gfx::RenderContext::API api;
		bool isRenderCtxInit = false, isApplicationInit;
		io::VirtualFileSystem vfs;
		gfx::RenderFactory factory;
		tui::Console console;
		io::FileLogger fLogger;
	};
}
