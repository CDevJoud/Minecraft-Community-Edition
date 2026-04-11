#pragma once
#include "Component.hpp"
#include "Core/FunctionContainer.hpp"

namespace mce::tui {
	class Panel : public virtual RenderTarget, public Component {
	public:
		class FunctionDefinition {
		public:
			using OnInit = eastl::function<void(Panel&)>;
			using OnRender = eastl::function<void(Panel&)>;
			using OnResize = eastl::function<void(Panel&)>;
		};
		using FD = FunctionDefinition;
		enum class TitleAlignment : int8_t {
			NONE = 0,
			Left,
			Center,
			Right
		};

		struct Properties {
			bool		   isResizable : 1 = true;
			bool		   isMovable : 1		 = true;
			int16_t		   titleAlignmentOffset  = 0;
			TitleAlignment titleAlignment		 = TitleAlignment::Center;
			std::string	   title				 = "";
			uint8_t		   borderColor			 = 0x0F;
		};

		Panel(core::QEventBus& qBus);
		Panel(core::QEventBus& qBus, const std::string& title, const uint16_t width, const uint16_t height, core::FunctionContainer fc = {});
		~Panel();
		static eastl::shared_ptr<Panel> createInstance(core::QEventBus& qBus, const std::string& title, const uint16_t width, const uint16_t height, core::FunctionContainer fc = {});
	
		bool create(const std::string& title, const uint16_t width, const uint16_t height);
		bool insertComponent(const eastl::shared_ptr<Component>& component);
		template<typename CompType>
		eastl::shared_ptr<CompType> getComponent(const std::string& title) {
			for (auto& i : this->components) {
				if (i->getID() == title) {
					return eastl::dynamic_pointer_cast<CompType>(i);
				}
			}
			return nullptr;
		}

		virtual void onUpdate(EventProcessor* ep) override;
		virtual void onRender(RenderTarget* out) override;
		virtual void onInit() override;

		sf::Vector2<short> getMousePosition() const;

		sf::Rect<short> getRect() const;

		Properties& getProperties();

		bool isFocused();
	protected:
		void setUpFrame(RenderTarget* out, sf::Rect<short> rect, uint8_t color);
	private:
		eastl::vector<eastl::shared_ptr<Component>> components;
		bool		   isResizingRight : 1 = false;
		bool		   isResizingLeft : 1 = false;
		bool		   isResizingBottom : 1 = false;
		Properties props;

		bool isDragging = false, resizeDragging = true, isHovering = false, targeted = false;
		sf::Vector2<short> offset{}, resizeOffset{}, mousePosition;
	};
}
