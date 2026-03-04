#pragma once

namespace MCE {
	class Renderer {
	public:
		virtual ~Renderer();
		virtual void renderFrame() = 0;
	};
}