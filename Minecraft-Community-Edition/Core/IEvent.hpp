#pragma once

namespace mce {
	struct IEvent {
		~IEvent() = default;
		virtual const char* name() const = 0;
	};
	namespace event {}
}
