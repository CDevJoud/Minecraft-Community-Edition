#pragma once
#include <source_location>

#include <EASTL/optional.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include "Common\Platform.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#undef ERROR
#endif

namespace mce {
	namespace core {
		class Thread;
	}
	using core::Thread;
	struct IEvent {
		virtual ~IEvent() = default;
		[[nodiscard]] virtual const char const* name() const = 0;
	};
	
	namespace event {
		struct LoggerOutput : IEvent {
			enum class Severity {
				INFO,
				WARN,
				ERROR,
				DEBUG,
				FATAL //Maybe
			};
			Severity severity;
			std::string msg;
			eastl::optional<std::source_location> location;

			explicit LoggerOutput(Severity s = Severity::INFO, const std::string m = "", eastl::optional<std::source_location> loc = eastl::nullopt)
				: severity(s), msg(m), location(loc) {}

			[[nodiscard]] virtual const char const* name() const override { return "mce.core.event.logger_output"; }
		};

		struct ThreadFinished : IEvent {
			ThreadFinished(Thread* ptr) : thread(ptr) {}
			Thread* thread;
			virtual const char const* name() const override { return "mce.core.event.thread_finished"; }
		};

		struct ThreadStarted : IEvent {
			ThreadStarted(Thread const* p, const eastl::string& n) : threadPtr(p), threadName(n) {}
			Thread const* threadPtr;
			const eastl::string threadName;
			virtual const char const* name() const override { return "mce.core.event.thread_started"; }
		};
	}
}
