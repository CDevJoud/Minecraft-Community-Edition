#pragma once
#include <RmlUi/Core.h>
#include <string>
#include <EASTL/unordered_map.h>
#include <EASTL/functional.h>
#include <EASTL/vector.h>
#include <Core/ResourceManager.hpp>

#include <mutex>

namespace mce {
	class Application;
}

namespace mce::ui {
	namespace priv {
		class EventListener : public Rml::EventListener {
		public:
			EventListener(eastl::function<void(Rml::Event&)> cb) : 
				cb(cb) {

			}

			void ProcessEvent(Rml::Event& event) override {
				if (EventListener::cb) {
					EventListener::cb(event);
				}
			}

		private:
			eastl::function<void(Rml::Event&)> cb;
		};
	}

	class UIManager {
	public:
		UIManager();
		~UIManager();

		bool init(core::ResourceManager& rm);
		void shutdown();
		
		using EventCB = eastl::function<void(Rml::Event&)>;

		/// Example usage:
		/// 
		/// 
		/// <button id="start_button">Start Game</button>
		/// 
		/// 
		/// ui.addEventListener("MainUI", "start_button", "click", [](Rml::Event& event) {
		///		std::cout << "Clicked!" << std::endl;
		/// }
		/// 
		void addEventCB(const std::string& ctxName, const std::string& elementId, const std::string& eventName, EventCB cb);

		bool createContext(const std::string& name, Rml::Vector2i dimensions);
		void destroyContext(const std::string& name);

		Rml::Context* getContext(const std::string& name);

		Rml::ElementDocument* loadDocument(const std::string& ctxName, const std::string& filePath);

		

	private:
		bool bInit = false;
		std::unordered_map<std::string, Rml::Context*> contexts;

		friend class mce::Application;

		void update();
		void render();

		void processEvents(sf::WindowHandle window, sf::Event& event);

		eastl::unique_ptr<Rml::RenderInterface> rmlRender = nullptr;
		eastl::unique_ptr<Rml::SystemInterface> rmlSystem = nullptr;
		eastl::unique_ptr<Rml::FileInterface> rmlFile = nullptr;
		eastl::vector<uint8_t> fontMem;

		eastl::vector<eastl::unique_ptr<priv::EventListener>> eventListeners;

		mutable std::mutex contextMutex;
	};
}
