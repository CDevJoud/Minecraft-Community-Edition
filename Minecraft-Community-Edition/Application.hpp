#pragma once
#include "Minecraft.hpp"
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>
#include "Core/QEventBus.hpp"
#include "Core/ThreadManager.hpp"
#include "SFML/Window/Window.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Graphics/Renderer.hpp"

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
		Application();

		/**
		 * @brief Starts the application.
		 *
		 * Launches all managed game instances and handles the main loop.
		 */
		virtual int run() override;

	private:

		
	private:
		
		eastl::vector<eastl::unique_ptr<Minecraft>> instances;
		std::mutex instanceMutex;
		QEventBus qLocalBus;
		ThreadManager threadManager;
	};
}
