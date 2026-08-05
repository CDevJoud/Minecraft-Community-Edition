#include "UIManager.hpp"
#include "Backend/RmlUI_Platform_dms.hpp"
#include "Backend/RmlUI_Renderer_bgfx.hpp"

namespace mce::ui {
	UIManager::UIManager() {
		
	}
	UIManager::~UIManager() {

	}

	bool UIManager::init(core::ResourceManager& rm) {
		if (UIManager::bInit) {
			return true;
		}

		UIManager::rmlRender = eastl::make_unique<priv::RenderInterface_bgfx>(rm);
		UIManager::rmlSystem = eastl::make_unique<priv::SystemInterface_dms>(rm.getQEventBus());
		UIManager::rmlFile = eastl::make_unique<priv::FileInterface_dms>(rm.getVirtualFileSystem());

		Rml::SetRenderInterface(UIManager::rmlRender.get());
		Rml::SetSystemInterface(UIManager::rmlSystem.get());
		Rml::SetFileInterface(UIManager::rmlFile.get());

		if (!Rml::Initialise()) {
			return false;
		}

		rm.getVirtualFileSystem().getFile("assets.fonts.arial_black", fontMem);
		Rml::Span<const Rml::byte> rmlFontMem(
			reinterpret_cast<const Rml::byte*>(fontMem.data()),
			fontMem.size()
		);

		Rml::LoadFontFace(rmlFontMem, "arial_black", Rml::Style::FontStyle::Normal);

		UIManager::bInit = true;

		return UIManager::bInit;
	}

	void UIManager::shutdown() {
		if (!UIManager::bInit) {
			return;
		}

		for (auto& [name, context] : UIManager::contexts) {
			Rml::RemoveContext(name);
		}

		UIManager::contexts.clear();

		rmlRender.release();
		rmlSystem.release();
		rmlFile.release();

		Rml::Shutdown(); 

		UIManager::bInit = false;
	}

	void UIManager::addEventCB(const std::string& name, const std::string& elementId, const std::string& eventName, EventCB cb) {
		Rml::Context* context = UIManager::getContext(name);

		if (!context) {
			return;
		}

		Rml::ElementDocument* doc = context->GetRootElement()->GetOwnerDocument();

		if (!doc) {
			return;
		}

		Rml::Element* element = doc->GetElementById(elementId);

		if (!element) {
			return;
		}

		auto listener = eastl::make_unique<priv::EventListener>(eastl::move(cb));
		
		element->AddEventListener(eventName, listener.get());

		UIManager::eventListeners.push_back(eastl::move(listener));
		Rml::Event e;
	}

	bool UIManager::createContext(const std::string& name, Rml::Vector2i size) {

		std::scoped_lock lock(contextMutex);
		if (UIManager::contexts.contains(name)) {
			return false;
		}

		Rml::Context* context = Rml::CreateContext(name, size);

		if (!context) {
			return false;
		}
		UIManager::contexts[name] = context;

		return true;
	}

	void UIManager::destroyContext(const std::string& name) {

		std::scoped_lock lock(contextMutex);
		auto it = UIManager::contexts.find(name);
		if (it == UIManager::contexts.end()) {
			return;
		}

		Rml::RemoveContext(name);

		UIManager::contexts.erase(it);
	}

	Rml::Context* UIManager::getContext(const std::string& name) {
		auto it = UIManager::contexts.find(name);

		if (it == UIManager::contexts.end()) {
			return nullptr;
		}

		return it->second;
	}

	Rml::ElementDocument* UIManager::loadDocument(const std::string& name, const std::string& path) {
		std::scoped_lock lock(contextMutex);
		Rml::Context* context = UIManager::getContext(name);

		if (!context) {
			return nullptr;
		}

		auto doc = context->LoadDocument(path);

		if (doc) {
			doc->Show();
		}

		return doc;
	}

	void UIManager::update() {
		std::scoped_lock lock(contextMutex);
		for (auto& [name, context] : UIManager::contexts) {
			context->Update();
		}
	}

	void UIManager::render() {
		std::scoped_lock lock(contextMutex);
		for (auto& [name, context] : UIManager::contexts) {
			context->Render();
		}
	}
	void UIManager::processEvents(sf::WindowHandle window, sf::Event& event) {

		for (auto context : contexts) {
			switch (event.type) {
			case sf::Event::MouseMoved: context.second->ProcessMouseMove(event.mouseMove.x, event.mouseMove.y, priv::getKeyModifierState()); break;
			case sf::Event::MouseButtonPressed: context.second->ProcessMouseButtonDown(event.mouseButton.button, priv::getKeyModifierState()); break;
			case sf::Event::MouseButtonReleased: context.second->ProcessMouseButtonUp(event.mouseButton.button, priv::getKeyModifierState()); break;
			case sf::Event::MouseWheelMoved: context.second->ProcessMouseWheel(float(-event.mouseWheel.delta), priv::getKeyModifierState()); break;
			case sf::Event::MouseLeft: context.second->ProcessMouseLeave(); break;
			case sf::Event::TextEntered:
			{
				Rml::Character character = Rml::Character(event.text.unicode);
				if (character == Rml::Character('\r'))
					character = Rml::Character('\n');

				if (event.text.unicode >= 32 || character == Rml::Character('\n'))
					 context.second->ProcessTextInput(character);
			}
			break;
			case sf::Event::KeyPressed: context.second->ProcessKeyDown(priv::convertKey(event.key.code), priv::getKeyModifierState()); break;
			case sf::Event::KeyReleased: context.second->ProcessKeyUp(priv::convertKey(event.key.code), priv::getKeyModifierState()); break;
			default: break;
			}
		}
	}
}
