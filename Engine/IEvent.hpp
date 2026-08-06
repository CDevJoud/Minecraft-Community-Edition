#pragma once
#include <source_location>

#include <EASTL/optional.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include "Platform.hpp"
#include <Core/WindowBase.hpp>
#include <Core/Vector2.hpp>
#ifdef MCE_PLATFORM_WINDOWS
#undef ERROR
#endif

namespace dms {
	namespace core {
		class Thread;
	}
	using core::Thread;
	struct IEvent {
		virtual ~IEvent() = default;
		[[nodiscard]] virtual const char const* name() const = 0;
	};
	
	namespace event {
		struct Log : IEvent {
			enum Severity {
				INFO = 0x02,
				WARN = 0x06,
				ERROR = 0x04,
				DEBUG = 0x07,
				FATAL = 0xFF//Maybe
			};
			Severity severity;
			std::string msg;
			std::string channel = "default";
			eastl::optional<std::source_location> location;

			explicit Log(Severity s = Severity::INFO, const std::string m = "", const std::string& channel = "default", eastl::optional<std::source_location> loc = eastl::nullopt)
				: severity(s), msg(m), channel(channel), location(loc) {}

			[[nodiscard]] virtual const char const* name() const override { return "dms.core.event.logger_output"; }
		};

		using LoggerOutput = Log;

		struct ThreadFinished : IEvent {
			ThreadFinished(Thread* ptr) : thread(ptr) {}
			Thread* thread;
			virtual const char const* name() const override { return "dms.core.event.thread_finished"; }
		};

		struct ThreadStarted : IEvent {
			ThreadStarted(Thread const* p, const eastl::string& n) : threadPtr(p), threadName(n) {}
			Thread const* threadPtr;
			const eastl::string threadName;
			virtual const char const* name() const override { return "dms.core.event.thread_started"; }
		};

		namespace window {
			struct Close : IEvent {
				Close(sf::WindowHandle window) : window(window) {}
				sf::WindowHandle window;
				virtual const char const* name() const override { return "dms.core.event.window.close"; }
			};

			struct Resize : IEvent {
				Resize(sf::WindowHandle window, sf::Vector2u newSize) : window(window), newSize(newSize) {}
				sf::WindowHandle window;
				sf::Vector2u newSize;
				virtual const char const* name() const override { return "dms.core.event.window.resize"; }
			};
		}
		namespace input {
			struct MouseMoved : IEvent {
				MouseMoved(sf::WindowHandle window, sf::Vector2i moved) : window(window), moved(moved) {}
				sf::WindowHandle window;
				sf::Vector2i moved;
				virtual const char const* name() const override { return "dms.core.event.input.mouse_moved"; }
			};
		}
	}
}
