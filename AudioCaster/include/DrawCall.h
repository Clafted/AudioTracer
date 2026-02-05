#pragma once

#include <raylib.h>
#include "Vec2.h"

// Class to store a raylib DrawLine() call
class DrawCall {

	Vec2 start, end;
	Color color = { 0,0,0 };

public:

	DrawCall() {}
	DrawCall(const Vec2& s, const Vec2& e, const Color& c)
		: start(s), end(e), color(c) {}

	inline Vec2 getStart() const {
		return start;
	}

	inline Vec2 getEnd() const {
		return end;
	}

	inline Color getColor() const {
		return color;
	}
};