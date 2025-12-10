#pragma once

#include "LineBuffer.hpp"
#include "Vec2.h"

class RayTracer {

public:

	int numRays = 0;
	int maxBounces = 4;

	/* Casts a ray from origin s, in direction d, with length t,
	* max-length cT, with numBounces bounces, from a buffer of
	* objects objects.
	* Returns a SoundInfo object of the resulting sound. */
	SoundInfo castRay(Vec2& s, 
		Vec2& d, 
		float t, 
		float cT, 
		int numBounces, 
		LineBuffer& objects);


	inline void resetTracer() {
		numRays = 0;
	}


private:

	/* Find the parameter of collision between a parametric line
	* and an implicit line, given tail s, direction d, implicit
	* line line, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	float getLineHit(Vec2& s, Vec2& d, LineObject& line, float t);


	/* Find the parameter of collision between a parametric line
	* and a sound point, given tail s, direction d, sound-point
	* sound, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	float getSoundHit(Vec2& s, Vec2& d, LineObject& sound, float rayLength);


	inline float getObjectHit(Vec2& s, Vec2& d, LineObject& object, float& t) {
		return (object.type == WALL) ? getLineHit(s, d, object, t) : getSoundHit(s, d, object, t);
	}


	inline float getVolumeWithFalloff(float maxRayLength, float currentRayLength) {
		return std::min(1.0f, 0.01f * (maxRayLength * maxRayLength) / (currentRayLength * currentRayLength));
	}


	/* Return whether or not a ray is approximately in an emitted
	* sound, given the active-time of sound soundActiveTime, and
	* of active-time of ray rayActiveTime.
	* Returns true if ray is within sound, false otherwise. */
	inline bool rayInSound(float soundActiveTime, float rayActiveTime) {
		return abs(soundActiveTime - rayActiveTime) <= 0.005f;
	}


	inline bool currentClosestIsShorter(float& currentClosest, float& compare) {
		return compare <= 0.1f || (currentClosest != -1.0f && currentClosest <= compare);
	}


	inline bool cantHearSound(SoundInfo& sound, float soundActiveTime, float rayActiveTime) {
		return sound.volume <= 0.0f || !rayInSound(soundActiveTime, rayActiveTime);
	}


	SoundInfo getSoundEmitted(LineObject& sound, 
		float currentTime, 
		float rayLength, 
		Vec2& s, 
		LineObject cL, 
		float p);


	SoundInfo getWallBouncedSound(LineObject& wall,
		LineObject& rayToWall,
		LineBuffer& objects,
		Vec2& rayDirection,
		float lengthToWall,
		float remainingRayLength,
		float totalRayLength,
		int numBounces,
		float fallOff);


	/* Finds the closest object collision between a parametric
	* line and a buffer of LineObjects, given tail s,
	* direction d, max parameter t, LineBuffer objects,
	* and closestDist, where closestDist is updated to parameter
	* for closest collision.
	* Returns a pointer to the closest object if any, nullptr otherwise. */
	LineObject* findClosestObject(Vec2& s, 
		Vec2& d, 
		float t, 
		LineBuffer& objects, 
		float& closestDist);

};