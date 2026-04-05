#pragma once
#include <cstdint>

namespace mce::gfx {
	class Renderer {
	public:
		Renderer(uint16_t viewId);
		void resize(float widht, float height);
		void updateView(float width, float height);
		
		void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);

		//void re

	private:
		uint16_t viewId;
	};
}
