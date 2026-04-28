#pragma once
#include "IConsole.hpp"
#include "Component.hpp"
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#include <IO/nlohmann/json.hpp>	

namespace mce::tui {
	class Console : public RenderTarget, public EventProcessor {
	public:
		enum class Type {
			None = 0x00,
			NativeWindows,
			Bgfx
		};

		Console(core::QEventBus& qBus, sf::WindowHandle window, const std::string& title, uint16_t width, uint16_t height, uint16_t pxlWidth, uint16_t pxlHeight, Type type = Type::NativeWindows);
		Console(const nlohmann::json& data);
		~Console();

		IConsole* getInterface();

		void* getInputHandle();
		void* getOutputHandle();
		
		bool insertComponent(const eastl::shared_ptr<Component>& component);

		template<typename CompType>
		eastl::shared_ptr<CompType> getComponent(const std::string& title) {
			for (auto& i : Console::components) {
				if (i->getID() == title) {
					return eastl::dynamic_pointer_cast<CompType>(i);
				}
			}
			return nullptr;
		}

		void display();
		bool isOpen();
		void close();
	private:
		IConsole* interface;
		Type type;

		eastl::vector<eastl::shared_ptr<Component>> components;

		void* hInput;
		void* hOutput;

		core::Rect<short> viewSpace;
	};
}
