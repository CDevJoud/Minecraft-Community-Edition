#include "CLogger.hpp"
#include "IEvent.hpp"

namespace mce::tui {

	static std::atomic_bool bIsEngineRunning = false;

	CLogger::CLogger(core::QEventBus& qBus) : Panel(qBus) {
		bIsEngineRunning = true;
	}

	CLogger::CLogger(core::QEventBus& qBus, const std::string& title,
		const uint16_t width, const uint16_t height,
		core::FunctionContainer fc)
		: Panel(qBus, title, width, height, fc) {
		
		bIsEngineRunning = true;
		
		//outputBuffer.reserve(MAX_BUFFER_SIZE);

		qBus.subscribe<event::LoggerOutput>([this](const event::LoggerOutput& e) {
			if (e.channel == Panel::getProperties().title) {
				e.severity;
				log(char(e.severity) + e.msg);
			}
			});
	}

	CLogger::~CLogger() {
		bIsEngineRunning.store(false, std::memory_order_release);
	}

	eastl::shared_ptr<CLogger> CLogger::createInstance(core::QEventBus& qBus,
		const std::string& title, const uint16_t width, const uint16_t height,
		core::FunctionContainer fc) {
		auto component = eastl::make_shared<CLogger>(qBus, title, width, height, fc);
		component->setPosition(1, 1);
		return component;
	}

	void CLogger::log(const std::string& message) {
		if(bIsEngineRunning.load(std::memory_order_relaxed)) {
			std::lock_guard<std::mutex> lock(bufferMutex);

			if (outputBuffer.size() >= MAX_BUFFER_SIZE) {
				outputBuffer.pop_front();           // Remove oldest message
			}

			outputBuffer.push_back(message);        // Add new message at the end
			// Auto scroll to bottom when new message arrives (only if not dragging)
			if (!isDraggingScrollbar)
				scrollToBottom();
		}
	}

	void CLogger::onRender(RenderTarget* out) {
		Panel::onRender(out);
		Panel::clear(0x2588, 0x11);

		std::lock_guard<std::mutex> lock(bufferMutex);

		const uint16_t w = getSize().x;
		const uint16_t h = getSize().y;

		if (w <= 2 || h <= 2) return;

		const size_t totalLines = outputBuffer.size();
		const int maxScroll = std::max(0, (int)totalLines - (int)h);

		verticalScroll = std::clamp(verticalScroll, 0, maxScroll);

		if (Panel::isFocused()) {

			// Draw the actual log lines
			for (uint16_t y = 0; y < h; ++y) {
				size_t lineIdx = verticalScroll + y;
				if (lineIdx >= totalLines)
					break;

				const std::string& text = outputBuffer[lineIdx];

				if (!text.empty() && horizontalScroll < (int)text.size()) {
					std::string visiblePart = text.substr(horizontalScroll,
						std::min((size_t)w - 1, text.size() - horizontalScroll));
					char color = visiblePart[0];
					Panel::renderText(0, y, visiblePart.data() + 1, 0x10 | color);        // This is correct
				}
			}

			// Vertical Scrollbar (using renderLine or loop with renderText)
			if (maxScroll > 0) {
				int thumbHeight = std::max(2, (h * h) / std::max(1, (int)totalLines));
				int thumbPos = (verticalScroll * (h - thumbHeight)) / std::max(1, maxScroll);

				// You can use renderLine for the background of scrollbar
				Panel::renderLine(w - 1, 0, w - 1, h - 1, 0x2502, 0x07);

				// Draw thumb
				Panel::renderLine(w - 1, thumbPos, w - 1, thumbPos + thumbHeight - 1, 0x2593, 0x06);
			}
			else {
				Panel::renderLine(w - 1, 0, w - 1, h - 1, 0x2502, 0x07);
			}


			out->setPixel(w - 1 + Panel::getPosition().x, Panel::getPosition().y - 1, 0x25B2, 0x03);
			out->setPixel(w - 1 + Panel::getPosition().x, h + Panel::getPosition().y, 0x25BC, 0x03);
		}
	}

	void CLogger::onUpdate(EventProcessor* ep) {
		Panel::onUpdate(ep);
		
		if (!ep) return;

		auto mousePos = ep->getMousePos();
		auto mouse = ep->Mouse(sf::Mouse::Left);

		const uint16_t panelX = getPosition().x;   // assuming Panel has getPosition()
		const uint16_t panelY = getPosition().y;
		const uint16_t panelW = getSize().x;
		const uint16_t panelH = getSize().y;

		// Convert global mouse pos to local panel coordinates
		int localX = mousePos.x - panelX;
		int localY = mousePos.y - panelY;
		if(Panel::isFocused())
		{

			// === Vertical Scrollbar Interaction ===
			const bool onScrollbar = (localX == panelW - 1) && (localY >= 0 && localY < panelH);

			if (mouse.bStrokePressed && onScrollbar) {
				isDraggingScrollbar = true;
				dragStartY = localY;
				dragStartScroll = verticalScroll;
			}
			else if (mouse.bStrokeReleased) {
				isDraggingScrollbar = false;
			}

			if (isDraggingScrollbar && mouse.bStrokeIsHeld) {
				std::lock_guard<std::mutex> lock(bufferMutex);
				int totalLines = (int)outputBuffer.size();
				int maxScroll = std::max(0, totalLines - (int)panelH);

				if (maxScroll > 0) {
					float scrollRatio = (float)localY / (panelH - 1);
					verticalScroll = (int)(scrollRatio * maxScroll);
					verticalScroll = std::clamp(verticalScroll, 0, maxScroll);
				}
			}

			// Optional: Click above/below scrollbar thumb to page up/down
			if (mouse.bStrokePressed && localX == panelW - 1) {
				std::lock_guard<std::mutex> lock(bufferMutex);
				int totalLines = (int)outputBuffer.size();
				int maxScroll = std::max(0, totalLines - (int)panelH);

				if (localY < (panelH / 2))
					verticalScroll = std::max(0, verticalScroll - (panelH / 2));           // Page Up
				else
					verticalScroll = std::min(maxScroll, verticalScroll + (panelH / 2));   // Page Down
			}
		}
	}

	void CLogger::scrollToBottom() {
		//std::lock_guard<std::mutex> lock(bufferMutex);
		verticalScroll = std::max(0, (int)outputBuffer.size() - (int)getSize().y);
	}

} // namespace mce::tui