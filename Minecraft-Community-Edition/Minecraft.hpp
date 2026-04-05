#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/string.h>
#include "SFML/Window/Window.hpp"
//#include "Graphics/GraphicsContext.hpp"
//#include "Graphics/Renderer.hpp"
#include "Core/QEventBus.hpp"
#include "Tasks.hpp"

namespace mce {
	namespace gfx {
		class RenderContext;
		class Renderer;
	}
	
	using core::QEventBus;
	using core::SubscriptionToken;
	using gfx::RenderContext;
	using tasks::RenderTask;
	class Minecraft {
	public:
		Minecraft(const eastl::string_view& profileName, QEventBus& qBus, uint16_t viewId, sf::WindowHandle window, sf::Vector2u viewSize, eastl::shared_ptr<RenderContext>& renderCtx);
		~Minecraft();

		int run();

		void render();

		bool isRunning() const { return this->bIsRunning; }

		const std::string& getProfileName() const { return this->profileName; }
	
	protected:
		friend class Application;
		uint16_t getViewId() const { return this->viewId; }
	private:

		int initInstance();

	private:
		bool bIsRunning;
		uint16_t viewId;


		QEventBus& qBus;
		sf::Vector2u viewSize;
		sf::WindowHandle window;

		const std::string profileName;
		const eastl::shared_ptr<RenderContext>& renderCtx;

		SubscriptionToken onResize, onClose;

	};
}
