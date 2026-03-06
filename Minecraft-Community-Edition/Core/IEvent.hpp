#pragma once

namespace mce {
	struct IEvent {
		virtual ~IEvent() = default;
		[[nodiscard]] virtual const char* name() const = 0;
	};
	namespace event {}
}
