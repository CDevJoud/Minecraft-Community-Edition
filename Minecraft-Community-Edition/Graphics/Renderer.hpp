#pragma once

namespace mce::gfx {
	class Renderer {
	public:
		virtual ~Renderer();
		virtual void renderFrame() = 0;
	};
}