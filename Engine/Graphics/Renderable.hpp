#pragma once
#include "RenderStates.hpp"

namespace dms::gfx {
	class Void;
	class Renderable {
	public:
		virtual void render(Void& v3d, RenderStates& states) const = 0;
	};
}
