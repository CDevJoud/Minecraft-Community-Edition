#pragma once
#include "Panel.hpp"
#include <vector>
#include <queue>

namespace dms::tui {

	class CLogger : public Panel {
	public:
		CLogger(core::QEventBus& qBus);
		CLogger(core::QEventBus& qBus, const std::string& title,
			const uint16_t width, const uint16_t height,
			core::FunctionContainer fc = {});

		~CLogger();

		static eastl::shared_ptr<CLogger> createInstance(core::QEventBus& qBus,
			const std::string& title, const uint16_t width, const uint16_t height,
			core::FunctionContainer fc = {});

		virtual void onRender(RenderTarget* out) override;
		virtual void onUpdate(EventProcessor* ep) override;

		void scrollUp(int lines = 1);
		void scrollDown(int lines = 1);
		void scrollToBottom();
		
	private:
		// Optional: Add log manually
		void log(const std::string& message);

		int verticalScroll = 0;
		int horizontalScroll = 0;

		// Mouse dragging state
		bool isDraggingScrollbar = false;
		int dragStartY = 0;
		int dragStartScroll = 0;

		std::deque<std::string> outputBuffer;
		std::mutex bufferMutex;           
		static constexpr size_t MAX_BUFFER_SIZE = 5000; // prevent unlimited growth
	};

} // namespace dms::tui