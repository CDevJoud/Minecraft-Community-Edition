#include "Component.hpp"

namespace mce::tui {
	Component::Component(core::QEventBus& qBus, Type cType, RenderElement* re, const std::string& id, core::FunctionContainer fc) :
		type(cType),
		CompRe(re),
		id(id),
		bTargeted(false),
		fc(fc),
		qBus(qBus) {

	}

	void Component::setSize(const uint16_t width, const uint16_t height, bool buf) {
		Component::rect.width = width;
		Component::rect.height = height;
		if (buf) {
			int size = width * height;
			if (Component::CompRe->buffer.size() != size) {
				Component::CompRe->viewSpace.width = width;
				Component::CompRe->viewSpace.height= height;
				Component::CompRe->buffer.shrink_to_fit();
				Component::CompRe->buffer.resize(size);
			}
		}
	}

	void Component::setPosition(const uint16_t x, const uint16_t y) {
		Component::rect.left = x;
		Component::rect.top  = y;
	}

	sf::Vector2<short> Component::getSize() const {
		return Component::rect.getSize();
	}

	sf::Vector2<short> Component::getPosition() const {
		return Component::rect.getPosition();
	}

	std::string Component::getID() const {
		return Component::id;
	}
	sf::Vector2<short> Component::getDefaultPosition() const {
		return Component::defaultPosition;
	}

	Component::Type Component::getType() const {
		return Component::type;
	}
	core::FunctionContainer& Component::getFunctionContainer() {
		return this->fc;
	}
	RenderElement* Component::getRenderElement() const {
		return this->CompRe;
	}
	void Component::setComponentType(Type type) {
		this->type = type;
	}
}
