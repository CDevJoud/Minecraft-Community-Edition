#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/string.h>
#include "SFML/Window/Window.hpp"
#include "Graphics/Void.hpp"
#include "Core/QEventBus.hpp"
#include "Tasks.hpp"
#include "IO/VirtualFileSystem.hpp"

namespace mce {
	namespace gfx {
		class RenderContext;
	}
	
	class Minecraft {
	public:
		Minecraft(const eastl::string_view& profileName, core::QEventBus& qBus, uint16_t viewId, sf::WindowHandle window, sf::Vector2u viewSize, eastl::shared_ptr<gfx::RenderContext>& renderCtx, gfx::RenderFactory& factory, io::VirtualFileSystem& vfs);
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
		gfx::Void renderer;
		uint16_t viewId;


		core::QEventBus& qBus;
		sf::Vector2u viewSize;
		sf::WindowHandle window;

		const std::string profileName;
		eastl::shared_ptr<gfx::RenderContext> renderCtx;

		core::SubscriptionToken onResize, onClose;
		io::VirtualFileSystem& vfs;
	};
}
