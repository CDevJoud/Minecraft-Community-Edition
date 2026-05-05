#pragma once
#include "RenderStates.hpp"

namespace mce::gfx {
	class Void;
	class Renderable {
	public:
		virtual void render(Void& v3d, RenderStates& states) const = 0;
	};
}
