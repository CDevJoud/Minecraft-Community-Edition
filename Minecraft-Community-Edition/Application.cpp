#include "Startup.hpp"

#include "Application.hpp"
#include <SFML/System/Sleep.hpp>

namespace mce {
	Application::Application() : 
		qLocalBus("APP"), 
		threadManager(qLocalBus) {

		//For now we will have a single profile
		Application::instances.emplace_back(eastl::make_unique<Minecraft>("MCE:Player1"));

		Minecraft* instance = instances.back().get();

		//create a new thread so we dont block the main thread
		threadManager.createThread("MCE:Player1", [this, instance]() {
			instance->run();

			std::lock_guard<std::mutex> lock(instanceMutex);

			auto it = eastl::find_if(instances.begin(), instances.end(), [instance](const eastl::unique_ptr<Minecraft>& m) {
				return m.get() == instance;
				});

			if (it != instances.end()) {
				instances.erase(it);
			}

			})->launch();
	}

	int Application::run() {
		while (true) {
			{
				std::lock_guard<std::mutex> lock(instanceMutex);

				if (instances.empty()) {
					break;
				}
			}

			sf::sleep(sf::milliseconds(10));
		}

		return 0;
	}
	
}

MCE_STARTUP(mce::Application);