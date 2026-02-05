#pragma once

#include <atomic>
#include "DrawCallBuffer.h"
#include "LineBuffer.hpp"
#include "Vec2.h"

class RayTracer {

public:

	static std::atomic_int numRays;
	static int maxBounces;

	/**
	* Casts a ray from origin s, in direction d, with length t,
	* max-length cT, with numBounces bounces, from a buffer of
	* objects objects.
	* 
	* @param s the origin of the ray
	* @param d the direction of the ray
	* @param t the length of the ray
	* @param cT the counting length of the ray (includes the length of connected rays)
	* @param numBounces the number of bounces remaining for the ray
	* @param objects the LineBuffer to iterate through collision calculations
	* @param drawBuffer the DrawCallBuffer to log ray collisions to
	* @return a SoundInfo object of the resulting sound. */
	static SoundInfo castRay(
		const Vec2& s, 
		const Vec2& d, 
		float t, 
		float cT, 
		int numBounces, 
		const LineBuffer& objects,
		DrawCallBuffer& drawBuffer);

	/** Reset RayTracer values for the next set of ray casts */
	static inline void resetTracer() {
		numRays.store(0);
	}


private:

	/**
	* Find the parameter of collision between a parametric line
	* and an implicit line, given tail s, direction d, implicit
	* line line, and max parameter t.
	* 
	* @param s the tail of the parametric line
	* @param d the direction of the parametric line
	* @param line the LineObject to check collision against
	* @param t the max parameter of the parametric line
	* @return a positive parameter for existing collision,
	* -1.0f otherwise. */
	static float getLineHit(const Vec2& s, const Vec2& d, const LineObject& line, float t);


	/**
	* Find the parameter of collision between a parametric line
	* and a sound point, given tail s, direction d, sound-point
	* sound, and max parameter t.
	* 
	* @param s the tail of the parametric line
	* @param d the direction of the parametric line
	* @param sound the LineObject sound point to check collision against
	* @param rayLength the length of the parametric line
	* @return a positive parameter for existing collision,
	* -1.0f otherwise. */
	static float getSoundHit(const Vec2& s, const Vec2& d, const LineObject& sound, float rayLength);


	static inline float getObjectHit(const Vec2& s, const Vec2& d, const LineObject& object, float& t) {
		return (object.type == WALL) ? getLineHit(s, d, object, t) : getSoundHit(s, d, object, t);
	}


	static inline float getVolumeWithFalloff(float maxRayLength, float currentRayLength) {
		return std::min(1.0f, 0.01f * (maxRayLength * maxRayLength) / (currentRayLength * currentRayLength));
	}


	/**
	* Return whether or not a ray is approximately in an emitted
	* sound, given the active-time of sound soundActiveTime, and
	* of active-time of ray rayActiveTime.
	* 
	* @param soundActiveTime the elapsed time since sound was emitted
	* @param rayActiveTime the elapsed time since ray was emitted
	* @return true if ray is within sound, false otherwise. */
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
		LineObject cL,
		DrawCallBuffer& drawBuffer,
		float p);


	static SoundInfo getWallBouncedSound(
		const LineObject& wall,
		const LineObject& rayToWall,
		const LineBuffer& objects,
		DrawCallBuffer& drawBuffer,
		const Vec2& rayDirection,
		float lengthToWall,
		float remainingRayLength,
		float totalRayLength,
		int numBounces,
		float fallOff);


	/**
	* Finds the closest object collision between a parametric
	* line and a buffer of LineObjects, given tail s,
	* direction d, max parameter t, LineBuffer objects,
	* and closestDist, where closestDist is updated to parameter
	* for closest collision.
	* 
	* @param s the tail of the parametric line
	* @param d the direction of the parametric line
	* @param t the max parameter of the parametric line
	* @param objects the LineBuffer to iterate through lines
	* @param closestDist float reference to update with closest distance
	* @return a pointer to the closest object if any, nullptr otherwise. */
	static const LineObject* findClosestObject(
		const Vec2& s, 
		const Vec2& d, 
		float t, 
		const LineBuffer& objects, 
		float& closestDist);

};