#pragma once
#include <source_location>

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
			std::source_location location;

			LoggerOutput(Severity s = Severity::INFO, const std::string m = "", std::source_location loc = std::source_location::current())
				: severity(s), msg(m), location(loc) {}

			virtual const char* name() const override { return "mce.core.event.logger_output"; }
		};
	}
}
