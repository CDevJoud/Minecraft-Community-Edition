#pragma once

namespace mce {
	class Renderer {
	public:
		virtual ~Renderer();
		virtual void renderFrame() = 0;
	};
}