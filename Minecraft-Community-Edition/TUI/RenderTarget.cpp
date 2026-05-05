#include "RenderTarget.hpp"
namespace mce::tui {
	RenderTarget::RenderTarget() : re(nullptr) {
	
	}
	RenderTarget::RenderTarget(RenderElement* re) : re(re) {

	}

	void RenderTarget::flushTo(RenderTarget* out, core::Rect<short> rect) {
		sf::Vector2<short> p1 = { rect.left, rect.top };
		sf::Vector2<short> p2 = { short(p1.x + rect.width), short(p1.y + rect.height) };
		for (int x = p1.x; x < p2.x; x++) {
			for (int y = p1.y; y < p2.y; y++) {
				int16_t px = (x - p1.x);
				int16_t py = (y - p1.y);

				auto pixel = RenderTarget::getPixelAt(px, py);
				out->setPixel(x, y, pixel.pixel.unicodeChar, pixel.attrib);
			}
		}
	}
	RenderTarget::~RenderTarget() {
		RenderTarget::re.buffer.clear();
		RenderTarget::re.buffer.shrink_to_fit();
	}
	void RenderTarget::setPixel(int16_t x, int16_t y, uint16_t c, uint16_t attrib) {
		if (x >= RenderTarget::re.viewSpace.left  &&
			x <  RenderTarget::re.viewSpace.width &&
			y >= RenderTarget::re.viewSpace.top   &&
			y <  RenderTarget::re.viewSpace.height ) {
			RenderTarget::re.buffer[y * RenderTarget::re.viewSpace.width + x].pixel.unicodeChar = c;
			RenderTarget::re.buffer[y * RenderTarget::re.viewSpace.width + x].attrib = attrib;
		}
	}
	void RenderTarget::fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t c, uint16_t attrib) {
		RenderTarget::calculateClipOn(x1, y1);
		RenderTarget::calculateClipOn(x2, y2);
		for (uint16_t h = y1; h < y2; h++) {
			for (uint16_t w = x1; w < x2; w++) {
				RenderTarget::setPixel(w, h, c, attrib);
			}
		}
	}
	void RenderTarget::clear(uint16_t c, uint16_t attrib) {
		RenderTarget::fill(0, 0, RenderTarget::re.viewSpace.width, RenderTarget::re.viewSpace.height, c, attrib);
	}
	void RenderTarget::renderLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t c, uint16_t attrib) {
		int32_t x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1; py = 2 * dx1 - dy1;
		if (dy1 <= dx1) {
			if (dx >= 0) {
				x = x1; y = y1; xe = x2;
			} else {
				x = x2; y = y2; xe = x1;
			}

			RenderTarget::setPixel(x, y, c, attrib);

			for (i = 0; x < xe; i++) {
				x = x + 1;
				if (px < 0) {
					px = px + 2 * dy1;
				} else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				RenderTarget::setPixel(x, y, c, attrib);
			}
		} else {
			if (dy >= 0) {
				x = x1; y = y1; ye = y2;
			}
			else {
				x = x2; y = y2; ye = y1;
			}

			RenderTarget::setPixel(x, y, c, attrib);

			for (i = 0; y < ye; i++) {
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				RenderTarget::setPixel(x, y, c, attrib);
			}
		}
	}
	void RenderTarget::renderText(int16_t x, int16_t y, const std::string& str, uint16_t attrib) {
		for (int i = 0; i < str.length(); i++) {
			RenderTarget::setPixel(x + i, y, str[i], attrib);
		}
	}
	void RenderTarget::renderTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c, uint16_t attrib) {
		RenderTarget::renderLine(x1, y1, x2, y2, c, attrib);
		RenderTarget::renderLine(x2, y2, x3, y3, c, attrib);
		RenderTarget::renderLine(x3, y3, x1, y1, c, attrib);
	}
	void RenderTarget::rasterizeTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c, uint16_t attrib) {
		auto SWAP = [](int16_t& x, int16_t& y) { int32_t t = x; x = y; y = t; };
		auto SWAP32 = [](int32_t& x, int32_t& y) { int32_t t = x; x = y; y = t; };
		auto drawline = [&](int32_t sx, int32_t ex, int32_t ny) { for (int32_t i = sx; i <= ex; i++) RenderTarget::setPixel(i, ny, c, attrib); };

		int32_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int32_t signx1, signx2, dx1, dy1, dx2, dy2;
		int32_t e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting poINTs
		dx1 = (int32_t)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int32_t)(y2 - y1);

		dx2 = (int32_t)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int32_t)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP32(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP32(dy2, dx2);
			changed2 = true;
		}

		e2 = (int32_t)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int32_t)(dx1 >> 1);

		for (int32_t i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max poINTs found on the y
			// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int32_t)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int32_t)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP32(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int32_t)(dx1 >> 1);

		for (int32_t i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}
	void RenderTarget::renderCircle(int16_t x, int16_t y, int16_t radius, uint16_t c, uint16_t attrib) {
	
	}
	void RenderTarget::calculateClipOn(int16_t& x, int16_t& y) {
		if (x < 0) x = 0;
		if (x > RenderTarget::re.viewSpace.width) x = RenderTarget::re.viewSpace.width;
		if (y < 0) y = 0;
		if (y > RenderTarget::re.viewSpace.height) y = RenderTarget::re.viewSpace.height;
	}
	bool RenderTarget::checkInBoundaries(int16_t x, int16_t y, core::Rect<short> rect) {
		return (x >= rect.left && x <= rect.width &&
				y >= rect.top  && y <= rect.height);
	}
	bool RenderTarget::checkInBoundaries(sf::Vector2<short> p, core::Rect<short> rect) {
		return (p.x >= rect.left && p.x <= rect.width &&
				p.y >= rect.top &&  p.y <= rect.height);
	}
	core::Rect<short> RenderTarget::getViewSpace() const {
		return RenderTarget::re.viewSpace;
	}
	CharInfo RenderTarget::getPixelAt(int16_t x, int16_t y) const {
		if (
			x >= RenderTarget::re.viewSpace.left  &&
			x <  RenderTarget::re.viewSpace.width &&
			y >= RenderTarget::re.viewSpace.top   &&
			y <  RenderTarget::re.viewSpace.height
			) {
			return RenderTarget::re.buffer[y * RenderTarget::re.viewSpace.width + x];
		}
		else {
			return CharInfo{};
		}
	}
	CharInfo* RenderTarget::getPixelBuffer() const {
		return const_cast<CharInfo*>(this->re.buffer.data());
	}
}
