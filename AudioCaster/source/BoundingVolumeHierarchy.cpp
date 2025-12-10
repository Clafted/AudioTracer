#include "../include/BoundingVolumeHierarchy.h"

void BoundingVolumeHierarchy::sortObjectsByXAxis(LineObject* objects, int numObjects, AXIS axis) {
	int jVal , lowestVal, lowestIdx;
	LineObject temp;
	for (int i = 0; i < numObjects; i++) {
		lowestIdx = i;
		lowestVal = (axis == X) 
					? (objects[i].start.x + objects[i].end.x) / 2
					: (objects[i].start.y + objects[i].end.y) / 2;
		for (int j = i + 1; j < numObjects; j++) {
			jVal = (axis == X) 
					? (objects[j].start.x + objects[j].end.x) / 2 
					: (objects[j].start.y + objects[j].end.y) / 2;
			if (jVal >= lowestVal) continue;
			lowestVal = jVal;
			lowestIdx = j;
		}
		temp = objects[i];
		objects[i] = objects[lowestIdx];
		objects[lowestIdx] = temp;
	}
}


void BoundingVolumeHierarchy::createBVH(LineObject* objects, int numObjects, BVHNode* dest, int depth) {
	 sortObjectsByXAxis(objects, numObjects, (depth % 2 == 0) ? X : Y); 
}