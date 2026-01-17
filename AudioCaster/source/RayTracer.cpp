#include "../include/RayTracer.h"
#include <mutex>

std::atomic_int RayTracer::numRays(0);
int RayTracer::maxBounces = 4;

std::mutex lock;

typedef std::pair<SoundInfo, float> EmittedSound;

float RayTracer::getLineHit(const Vec2& s, const Vec2& d, const LineObject& line, float t) {
	float dP = Vec2::dot(d, line.normal);
	if (dP == 0.0f) return -1.0f;	// Parallel to line (orthogonal to normal), no collision
	float collision = Vec2::dot(line.end - s, line.normal) / dP;
	return (0.0f < collision && collision <= t
		&& line.containsPoint(s + d * collision))
		? collision : -1.0f;
}

float RayTracer::getSoundHit(const Vec2& s, const Vec2& d, const LineObject& sound, float rayLength) {
	float distanceFromStartToSound = LineObject::getLength(s, sound.start);
	if (rayLength < distanceFromStartToSound) return -1.0f;
	float distanceFromEndToSound = LineObject::getLength(s + d * distanceFromStartToSound, sound.start);
	return (distanceFromEndToSound <= sound.radius) ? distanceFromStartToSound : -1.0f;
}

const LineObject* RayTracer::findClosestObject(const Vec2& s, const Vec2& d, float t, const LineBuffer& objects, float& closestDist) {
	float distance;
	int closestIdx = 0;
	closestDist = -1.0f;
	for (int i = 0; i < objects.lineCount; i++) {
		distance = getObjectHit(s, d, objects.lines[i], t);
		if (currentClosestIsShorter(closestDist, distance)) continue;
		closestDist = distance;
		closestIdx = i;
	}
	return &objects.lines[closestIdx];
}

SoundInfo RayTracer::castRay(
	const Vec2& s, 
	const Vec2& d, 
	float t, 
	float cT, 
	int numBounces, 
	const LineBuffer& objects,
	DrawCallBuffer& drawBuffer) {

	if (numBounces >= maxBounces) return SoundInfo{};
	numRays++;
	float closestDistance;
	const LineObject* closestObject = findClosestObject(s, d, t, objects, closestDistance);
	if (!closestObject || closestDistance == -1.0f || t < closestDistance) {

		drawBuffer.addCall(DrawCall(Vec2((int)s.x, (int)s.y), 
									Vec2((int)s.x + d.x * t, (int)s.y + d.y * t), 
									Color{ 255, 50, 50, 40 }));

		return SoundInfo{};
	}
	LineObject lineToClosestObject{ s, s + d * closestDistance };
	float rayLength = cT + closestDistance;
	float volume = getVolumeWithFalloff(t + cT, rayLength);
	return (closestObject->type == SOUND)
		? getSoundEmitted(*closestObject, GetTime(), rayLength, s, lineToClosestObject, drawBuffer, volume)
		: getWallBouncedSound(*closestObject, lineToClosestObject, objects, drawBuffer, d, closestDistance, t, cT, numBounces, volume);
}

SoundInfo RayTracer::getWallBouncedSound(
	const LineObject& wall,
	const LineObject& rayToWall,
	const LineBuffer& objects,
	DrawCallBuffer& drawBuffer,
	const Vec2& rayDirection,
	float lengthToWall,
	float remainingRayLength,
	float totalRayLength,
	int numBounces,
	float fallOff) {

	drawBuffer.addCall(DrawCall(Vec2(rayToWall.start.x, rayToWall.start.y), 
								Vec2(rayToWall.end.x, rayToWall.end.y), 
								Color{ 40, 140, 250, (unsigned char)(fallOff * 255) }));

	Vec2 normal = wall.normal;
	float dP = Vec2::dot(rayDirection, normal);
	if (dP > 0) {
		normal *= -1;
		dP *= -1;
	}
	Vec2 reflection = rayDirection - normal * (2 * dP);
	SoundInfo reflS = castRay(rayToWall.end, reflection, remainingRayLength - lengthToWall, totalRayLength, numBounces + 1, objects, drawBuffer);
	SoundInfo refrS = castRay(rayToWall.end, rayDirection, remainingRayLength - lengthToWall, totalRayLength, numBounces + 1, objects, drawBuffer);
	return reflS * wall.reflection + refrS * wall.refraction;
}

SoundInfo RayTracer::getSoundEmitted(
	const LineObject& sound,
	float currentTime,
	float rayLength,
	const Vec2& s,
	LineObject cL,
	DrawCallBuffer& drawBuffer,
	float p) {

	drawBuffer.addCall(DrawCall(Vec2((int)s.x, (int)s.y), 
								Vec2((int)cL.end.x, (int)cL.end.y), 
								Color{ 200, 200, 100, (unsigned char)(p * 255) }));

	EmittedSound emittedSound;
	for (int i = 0; i < sound.numActive; i++) {
		emittedSound = sound.activeSounds[i];
		if (cantHearSound(emittedSound.first, currentTime - emittedSound.second, rayLength / SOUND_SPEED)) continue;
		emittedSound.first.volume = p;

		drawBuffer.addCall(DrawCall(Vec2((int)s.x, (int)s.y), 
									Vec2((int)cL.end.x, (int)cL.end.y), 
									GREEN));

		return emittedSound.first;
	}
	return SoundInfo{};
}