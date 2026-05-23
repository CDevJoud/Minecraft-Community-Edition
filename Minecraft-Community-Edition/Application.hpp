#pragma once
#include "Minecraft.hpp"
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>
#include "Core/QEventBus.hpp"
#include "Core/ThreadManager.hpp"
#include "Core/WindowBase.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderFactory.hpp"
#include "libs/bx/commandline.h"
#include "IO/VirtualFileSystem.hpp"
#include "tui/Console.hpp"
#include "IO/FileLogger.hpp"
#include "Core/ResourceManager.hpp"
#include "UI/Backend/RmlUI_Renderer_bgfx.hpp"

#include <RmlUi/Core.h>
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

		core::QEventBus qBus;

	private:
		std::string getLogFileName();

		void setupLogging();

		bool initApplication();
		
	private:
		void initQEventBusSubscription();
		void createProfile(const eastl::string profileName, eastl::unique_ptr<sf::WindowBase> window);

		eastl::unique_ptr<sf::WindowBase> appWindow = nullptr;

		eastl::vector<eastl::pair<eastl::unique_ptr<sf::WindowBase>, eastl::unique_ptr<Minecraft>>> instances;

		std::mutex instanceMutex;
		core::ThreadManager threadManager;
		eastl::shared_ptr<gfx::RenderContext> renderCtx;
		bx::CommandLine cmd;
		gfx::RenderContext::API api;
		bool isRenderCtxInit = false, isApplicationInit;
		io::VirtualFileSystem vfs;
		gfx::RenderFactory factory;
		tui::Console console;
		io::FileLogger fLogger;

		Rml::SystemInterface* rmlSystem = nullptr;
		ui::priv::RenderInterface_bgfx* rmlRenderer = nullptr;
		Rml::Context* ctx;

		eastl::unique_ptr<core::ResourceManager> rsrcMgr = nullptr;
		Rml::ElementDocument* doc;
		eastl::vector<uint8_t> fontMem;
	};
}

