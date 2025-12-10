#pragma once

#include "BVHNode.h"

enum AXIS { X, Y };

class BoundingVolumeHierarchy {

	BVHNode* root;

	void sortObjectsByXAxis(LineObject* objects, int numObjects, AXIS axis);

public:

	void createBVH(LineObject* objects, int numObjects, BVHNode* dest, int depth = 0);

};