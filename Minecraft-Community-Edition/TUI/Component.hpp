#pragma once
#include "RenderTarget.hpp"
#include "Core/FunctionContainer.hpp"
#include "EventProcessor.hpp"

namespace mce::tui {
	class Component {
	public:
		enum Type {
			None     = 0x00,
			Button   = 0b0001,
			Panel    = 0b0010,
			InputBox = 0b0100
		};

		Component(core::QEventBus& qBus, Type cType, RenderElement* re, const std::string& id, core::FunctionContainer fc = {});

		virtual void onUpdate(EventProcessor* ep) = 0;
		virtual void onRender(RenderTarget* out) = 0;
		virtual void onInit() = 0;

		virtual void setSize(const uint16_t width, const uint16_t height, bool resizeBuffer = false);
		virtual void setPosition(const uint16_t x, const uint16_t y);

		sf::Vector2<short> getPosition() const;
		sf::Vector2<short> getSize() const;
		std::string getID() const;

		sf::Vector2<short> getDefaultPosition() const;
		Type getType() const;

		core::FunctionContainer& getFunctionContainer();

	protected:
		core::FunctionContainer fc;

		bool bTargeted;
		RenderElement* getRenderElement() const;
		void setComponentType(Type type);

		core::Rect<short> rect;

		core::QEventBus& qBus;
	private:
		std::string id;
		sf::Vector2<short> defaultPosition;
		Type type;
		RenderElement* CompRe;
		friend class Console;
	};
}
