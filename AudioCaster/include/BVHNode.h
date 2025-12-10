#pragma once

#include "Vec2.h"
#include "LineBuffer.hpp"

struct BoundingVolume {
	Vec2 position, size;
};

struct BVHNode {
	BoundingVolume bv;
	BVHNode *left, *right;
};