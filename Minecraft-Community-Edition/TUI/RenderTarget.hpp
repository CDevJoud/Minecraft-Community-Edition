#pragma once
#include <EASTL/vector.h>
#include <Core/Rect.hpp>
#include <string>

namespace mce::tui {
	struct CharInfo {
		union {
			wchar_t unicodeChar;
			char asciiChar;
		}pixel;
		short attrib;
	};

	struct RenderElement {
		void* hConsole;
		eastl::vector<CharInfo> buffer;
		core::Rect<short> viewSpace;
	};
	
	class RenderTarget {
	public:
		RenderTarget();
		RenderTarget(RenderElement* re);
		void flushTo(RenderTarget* out, core::Rect<short> rect);
		~RenderTarget();

		void setPixel(int16_t x, int16_t y, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);
		void fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);

		void clear(uint16_t c = 0x2588, uint16_t attrib = 0x00);

		void renderLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);
		void renderText(int16_t x, int16_t y, const std::string& str, uint16_t attrib = 0x000F);
		void renderTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);
		void rasterizeTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);
		
		[[deprecated]]
		void renderCircle(int16_t x, int16_t y, int16_t radius, uint16_t c = 0x2588, uint16_t attrib = 0x00FF);

		void calculateClipOn(int16_t& x, int16_t& y);
		bool checkInBoundaries(int16_t x, int16_t y, core::Rect<short> rect);
		bool checkInBoundaries(sf::Vector2<short> p, core::Rect<short> rect);
		core::Rect<short> getViewSpace() const;

	protected:
		RenderElement re;
		friend class Console;
		CharInfo getPixelAt(int16_t x, int16_t y) const;
		CharInfo* getPixelBuffer() const;
	};
}
