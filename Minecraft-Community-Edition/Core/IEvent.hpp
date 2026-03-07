#pragma once
#include <source_location>

#include <EASTL/optional.h>

namespace mce {
	struct IEvent {
		virtual ~IEvent() = default;
		[[nodiscard]] virtual const char* name() const = 0;
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

			[[nodiscard]] const char* name() const override { return "mce.core.event.logger_output"; }
		};
	}
}
