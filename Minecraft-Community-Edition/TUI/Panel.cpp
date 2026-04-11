#include "Panel.hpp"

namespace mce::tui {
	Panel::Panel(core::QEventBus& qBus) : 
		Component(qBus, Component::Type::Panel, &re, "") {

	}
	Panel::Panel(core::QEventBus& qBus, const std::string& title, const uint16_t width, const uint16_t height, core::FunctionContainer fc) : 
		Component(qBus, Component::Type::Panel, &re, title, fc) {
		Panel::create(title, width, height);

	}
	Panel::~Panel() {
	}

	eastl::shared_ptr<Panel> Panel::createInstance(core::QEventBus& qBus, const std::string& title, const uint16_t width, const uint16_t height, core::FunctionContainer fc) {
		auto component = eastl::make_shared<Panel>(qBus, title, width, height, fc);
		if (component->re.buffer.empty()) {
			return nullptr;
		}
		component->setPosition(1, 1);
		return component;
	}

	bool Panel::create(const std::string& title, const uint16_t width, const uint16_t height) {
		if (!RenderTarget::checkInBoundaries((short)width, (short)height , { 0, 0, 512, 512 })) {
			return false;
		}
		else {
			Component::rect.width = width;
			Component::rect.height = height;
			Panel::props.title = title;
			RenderTarget::re.buffer.resize(width * height);
			RenderTarget::re.viewSpace = { 0, 0, (short)width, (short)height };
			return true;
		}
	}

	bool Panel::insertComponent(const eastl::shared_ptr<Component>& component) {
		auto cType = component->getType();
		switch (cType) {
		case mce::tui::Component::None:
			break;
		case mce::tui::Component::Button:
		{
			auto lastComponent = (this->components.empty()) ? eastl::shared_ptr<Component>() : *(eastl::prev(this->components.end()));
			if (lastComponent == nullptr) {
				component->setPosition(0, 0);
				this->components.push_back(component);
				component->onInit();
				return true;
			}
			else {
				if ((lastComponent->getType() & Component::Type::Panel) != Component::Type::None) {
					component->setPosition(0, lastComponent->getPosition().y + 2);
				}
				else {
					component->setPosition(0, lastComponent->getPosition().y + 1);

				}
				this->components.push_back(component);
				component->onInit();
				return true;
			}
		}
			break;
		case mce::tui::Component::Panel:
			components.push_back(component);
			break;
		case mce::tui::Component::InputBox:
			break;
		default:
			break;
		}

		//Mixed Types
		if ((cType & (Component::Type::Panel | Component::Type::InputBox)) != Component::Type::None) {
			auto lastComponent = (this->components.empty()) ? eastl::shared_ptr<Component>() : *(eastl::prev(this->components.end()));
			if (lastComponent == nullptr) {
				component->setPosition(1, 1);
				this->components.push_back(component);
				component->onInit();
				return true;
			}
			else {
				component->setPosition(1, lastComponent->getPosition().y + 1);
				this->components.push_back(component);
				component->onInit();
				return true;
			}
		}
	}

	void Panel::onUpdate(EventProcessor* ep) {
		if (this->fc.findFunction("OnUpdate"))
			this->fc.callFunction<void>("OnUpdate");
		//Update Components
		for (int i = 0; i < this->components.size(); i++) {
			auto& component = this->components[i];
			EventProcessor epInstance = *ep;
			epInstance.mousePos.x = ep->getMousePos().x - this->getPosition().x;
			epInstance.mousePos.y = ep->getMousePos().y - this->getPosition().y;
			component->onUpdate(&epInstance);
		}

		// Check if the mouse is hovering over the panel
		if (this->checkInBoundaries(ep->getMousePos(), {
				this->getPosition().x,
				this->getPosition().y,
				short(this->getPosition().x + this->getSize().x),
				short(this->getPosition().y + this->getSize().y)
			})) {
			sf::Vector2<short> mPos = {
				short(ep->getMousePos().x - this->getPosition().x),
				short(ep->getMousePos().y - this->getPosition().y)
			};
			this->mousePosition = mPos;

			if (this->fc.findFunction("OnMouseHover")) {
				this->fc.callFunction<void, const sf::Vector2<short>& > ("OnMouseHover", mPos);
			}
			this->isHovering = true;
		}
		else {
			this->isHovering = false;
		}

		// Handle mouse press events
		if (ep->Mouse(sf::Mouse::Button::Left).bStrokePressed) {
			if (Panel::props.isMovable) {
				// Dragging logic
				if (this->checkInBoundaries(ep->getMousePos(), {
					this->getPosition().x,
					short(this->getPosition().y - 1),
					short(this->getPosition().x + this->getSize().x),
					short(this->getPosition().y - 1)
					})) {
					isDragging = true;
					offset = {
						short(ep->getMousePos().x - this->getPosition().x),
						short(ep->getMousePos().y - this->getPosition().y)
					};
				}
			}

			// Targeting logic
			this->targeted = this->isHovering;

			// Resizing logic
			const short resizeMargin = 0; // Buffer zone for resizing interaction

			if (Panel::props.isResizable) {
				// Right edge resizing
				if (this->checkInBoundaries(ep->getMousePos(), {
					short(this->getPosition().x + this->getSize().x - resizeMargin),
					this->getPosition().y,
					short(this->getPosition().x + this->getSize().x),
					short(this->getPosition().y + this->getSize().y - 1)
					})) {
					resizeDragging = true;
					this->isResizingRight = true;
					this->resizeOffset = {
						short(ep->getMousePos().x - this->getPosition().x),
						this->getSize().y
					};
				}
				// Bottom edge resizing
				else if (this->checkInBoundaries(ep->getMousePos(), {
					short(this->getPosition().x + 1),
					short(this->getPosition().y + this->getSize().y - resizeMargin),
					short(this->getPosition().x + this->getSize().x - 1),
					short(this->getPosition().y + this->getSize().y)
					})) {
					resizeDragging = true;
					this->isResizingBottom = true;
					this->resizeOffset = {
						this->getSize().x,
						short(ep->getMousePos().y - this->getPosition().y)
					};
				}
				// Left edge resizing
				else if (this->checkInBoundaries(ep->getMousePos(), {
					short(this->getPosition().x - 1),
					short(this->getPosition().y),
					short(this->getPosition().x + resizeMargin - 1),
					short(this->getPosition().y + this->getSize().y - 1)
					})) {
					this->isResizingLeft = true;
					resizeDragging = true;
					this->resizeOffset = {
						short(ep->getMousePos().x - this->getPosition().x + 1),
						this->getSize().y
					};
				}
				else if (this->getPosition().x + this->getSize().x == ep->getMousePos().x &&
					this->getPosition().y + this->getSize().y == ep->getMousePos().y) {
					this->isResizingRight = this->isResizingBottom = true;
					this->resizeDragging = true;
					this->resizeOffset = {
						short(ep->getMousePos().x - this->getPosition().x),
						short(ep->getMousePos().y - this->getPosition().y)
					};
				}
				else if (this->getPosition().x - 1 == ep->getMousePos().x &&
					this->getPosition().y + this->getSize().y == ep->getMousePos().y) {
					this->isResizingLeft = this->isResizingBottom = true;
					this->resizeDragging = true;
					this->resizeOffset = {
						short(ep->getMousePos().x - this->getPosition().x),
						short(ep->getMousePos().y - this->getPosition().y)
					};
				}
			}
		}

		// Handle dragging
		if (ep->Mouse(sf::Mouse::Button::Left).bStrokeIsHeld) {
			if (isDragging) {
				this->setPosition(
					ep->getMousePos().x - offset.x,
					ep->getMousePos().y - offset.y
				);
			}
			if (resizeDragging) {
				// Minimum size constraints
				short minWidth = 0;
				if (Panel::props.titleAlignment == TitleAlignment::Center) {
					minWidth = Panel::props.title.length() * 4; // Minimum width
				}
				else if (Panel::props.titleAlignment == TitleAlignment::Left) {
					minWidth = Panel::props.title.length() * 2; // Minimum width
				}
				else {
					minWidth = Panel::props.title.length() * 2; // Minimum width
				}

				const short minHeight = 1;                          // Minimum height for the panel

				short newX = this->getPosition().x;  // Default to current X position
				short newWidth = this->getSize().x; // Default to current width
				short newHeight = this->getSize().y; // Default to current height

				// Right-edge resizing
				if (isResizingRight) {
					newWidth = ep->getMousePos().x - this->getPosition().x;
					if (newWidth < minWidth)
						newWidth = minWidth;
					//this->isResizingRight = false;
				}

				// Left-edge resizing
				if (isResizingLeft) {
					short leftDelta = this->getPosition().x - ep->getMousePos().x;
					newWidth = this->getSize().x + leftDelta;

					if (newWidth >= minWidth) {
						// Adjust position only if the width is valid
						newX = ep->getMousePos().x;
					}
					else {
						// Prevent width from going below the minimum
						newWidth = minWidth;
						newX = this->getPosition().x + this->getSize().x - minWidth;
					}
				}

				// Bottom-edge resizing
				if (isResizingBottom) {
					newHeight = ep->getMousePos().y - this->getPosition().y;
					if (newHeight < minHeight)
						newHeight = minHeight;
				}

				// Apply new position and size
				this->setPosition(newX, this->getPosition().y);
				this->setSize(newWidth, newHeight, true);
				if (this->getFunctionContainer().findFunction("OnResize"))
					this->getFunctionContainer().callFunction<void, Panel&>("OnResize", *this);
			}
		}

		// Handle mouse release
		if (ep->Mouse(sf::Mouse::Button::Left).bStrokeReleased) {
			isDragging = false;
			resizeDragging = false;
			this->isResizingLeft = this->isResizingBottom = this->isResizingRight = false;
		}
	}

	void Panel::onRender(RenderTarget* out) {
		if (this->fc.findFunction("OnLastRender"))
			this->fc.callFunction<void>("OnLastRender");
		for (int i = 0; i < this->components.size(); i++) {
			auto& component = this->components[i];

			component->onRender(this);
		}
		//int a = this->GetTitle().length() * (this->GetTitle().length() / this->GetSize().X);
		Panel::setUpFrame(out, this->getRect(), Panel::props.borderColor);
		switch (Panel::props.titleAlignment) {
		default:
			break;
		case TitleAlignment::Left:
			out->renderText(this->getPosition().x + Panel::props.titleAlignmentOffset + (Panel::props.title.length() * 0.25),
				this->getPosition().y - 1, Panel::props.title, 0x0f);
			break;

		case TitleAlignment::Center:
			out->renderText(this->getPosition().x + (this->getSize().x / 2u) - (Panel::props.title.length() / 2),
				this->getPosition().y - 1, Panel::props.title, 0x0f);
			break;

		case TitleAlignment::Right:
			out->renderText(this->getPosition().x + this->getSize().x + Panel::props.titleAlignmentOffset - (Panel::props.title.length() * 1.25),
				this->getPosition().y - 1, Panel::props.title, 0x0f);
			break;
		}

		RenderTarget::flushTo(out, this->getRect());
		if (this->fc.findFunction("OnRender"))
			this->fc.callFunction<void, Panel&>("OnRender", *this);
	}

	void Panel::onInit() {
		if (this->fc.findFunction("OnInit"))
			this->fc.callFunction<void, Panel&>("OnInit", *this);
	}
	sf::Vector2<short> Panel::getMousePosition() const {
		return Panel::mousePosition;
	}
	sf::Rect<short> Panel::getRect() const {
		return sf::Rect<short>(Component::getPosition(), Component::getSize());
	}
	Panel::Properties& Panel::getProperties() {
		// TODO: insert return statement here
		return Panel::props;
	}
	void Panel::setUpFrame(RenderTarget* out, sf::Rect<short> rect, uint8_t color) {
		uint16_t x = rect.left;
		uint16_t y = rect.top;
		uint16_t width = rect.width;
		uint16_t height = rect.height;
		// Top Horizontal Line
		out->renderLine(x - 1, y - 1, x + width, y - 1, 0x2500, color);

		// Left Vertical Line
		out->renderLine(x - 1, y - 1, x - 1, y + height - 1, 0x2502, color);

		// Bottom Horizontal Line
		out->renderLine(x - 1, y + height, x + width, y + height, 0x2500, color);

		// Right Vertical Line
		out->renderLine(x + width, y - 1, x + width, y + height - 1, 0x2502, color);

		// Corner Characters
		out->setPixel(x - 1, y - 1, 0x256D, color);           // Top-Left
		out->setPixel(x + width, y - 1, 0x256E, color);       // Top-Right
		out->setPixel(x - 1, y + height, 0x2570, color);      // Bottom-Left
		out->setPixel(x + width, y + height, 0x256F, color);  // Bottom-Right
	}
}

