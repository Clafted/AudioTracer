#pragma once

#include "LineBuffer.hpp"
#include "Vec2.h"

static class RayTracer {

public:

	static int numRays;
	static int maxBounces;

	/* Casts a ray from origin s, in direction d, with length t,
	* max-length cT, with numBounces bounces, from a buffer of
	* objects objects.
	* Returns a SoundInfo object of the resulting sound. */
	static SoundInfo castRay(
		const Vec2& s, 
		const Vec2& d, 
		float t, 
		float cT, 
		int numBounces, 
		const LineBuffer& objects);


	static inline void resetTracer() {
		numRays = 0;
	}


private:

	/* Find the parameter of collision between a parametric line
	* and an implicit line, given tail s, direction d, implicit
	* line line, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	static float getLineHit(const Vec2& s, const Vec2& d, const LineObject& line, float t);


	/* Find the parameter of collision between a parametric line
	* and a sound point, given tail s, direction d, sound-point
	* sound, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	static float getSoundHit(const Vec2& s, const Vec2& d, const LineObject& sound, float rayLength);


	static inline float getObjectHit(const Vec2& s, const Vec2& d, const LineObject& object, float& t) {
		return (object.type == WALL) ? getLineHit(s, d, object, t) : getSoundHit(s, d, object, t);
	}


	static inline float getVolumeWithFalloff(float maxRayLength, float currentRayLength) {
		return std::min(1.0f, 0.01f * (maxRayLength * maxRayLength) / (currentRayLength * currentRayLength));
	}


	/* Return whether or not a ray is approximately in an emitted
	* sound, given the active-time of sound soundActiveTime, and
	* of active-time of ray rayActiveTime.
	* Returns true if ray is within sound, false otherwise. */
	static inline bool rayInSound(float soundActiveTime, float rayActiveTime) {
		return abs(soundActiveTime - rayActiveTime) <= 0.005f;
	}


	static inline bool currentClosestIsShorter(float& currentClosest, float& compare) {
		return compare <= 0.1f || (currentClosest != -1.0f && currentClosest <= compare);
	}


	static inline bool cantHearSound(const SoundInfo& sound, float soundActiveTime, float rayActiveTime) {
		return sound.volume <= 0.0f || !rayInSound(soundActiveTime, rayActiveTime);
	}


	static SoundInfo getSoundEmitted(
		const LineObject& sound, 
		float currentTime, 
		float rayLength, 
		const Vec2& s, 
		const LineObject cL, 
		float p);


	static SoundInfo getWallBouncedSound(
		const LineObject& wall,
		const LineObject& rayToWall,
		const LineBuffer& objects,
		const Vec2& rayDirection,
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
	static const LineObject* findClosestObject(
		const Vec2& s, 
		const Vec2& d, 
		float t, 
		const LineBuffer& objects, 
		float& closestDist);

};