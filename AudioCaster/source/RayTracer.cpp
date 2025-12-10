#include "../include/RayTracer.h"

typedef std::pair<SoundInfo, float> EmittedSound;


float RayTracer::getLineHit(Vec2& s, Vec2& d, LineObject& line, float t) {
	float dP = Vec2::dot(d, line.normal);
	if (dP == 0.0f) return -1.0f;	// Parallel to line (orthogonal to normal), no collision
	float collision = Vec2::dot(line.end - s, line.normal) / dP;
	return (0.0f < collision && collision <= t
		&& line.containsPoint(s + d * collision))
		? collision : -1.0f;
}

float RayTracer::getSoundHit(Vec2& s, Vec2& d, LineObject& sound, float rayLength) {
	float distanceFromStartToSound = LineObject::getLength(s, sound.start);
	if (rayLength < distanceFromStartToSound) return -1.0f;
	float distanceFromEndToSound = LineObject::getLength(s + d * distanceFromStartToSound, sound.start);
	return (distanceFromEndToSound <= sound.radius) ? distanceFromStartToSound : -1.0f;
}

LineObject* RayTracer::findClosestObject(Vec2& s, Vec2& d, float t, LineBuffer& objects, float& closestDist) {
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

SoundInfo RayTracer::castRay(Vec2& s, Vec2& d, float t, float cT, int numBounces, LineBuffer& objects) {
	if (numBounces >= maxBounces) return SoundInfo{};
	numRays++;
	float closestDistance;
	LineObject* closestObject = findClosestObject(s, d, t, objects, closestDistance);
	if (!closestObject || closestDistance == -1.0f || t < closestDistance) {
		DrawLine((int)s.x, (int)s.y, (int)s.x + d.x * t, (int)s.y + d.y * t, Color{ 255, 50, 50, 40 });
		return SoundInfo{};
	}
	LineObject lineToClosestObject{ s, s + d * closestDistance };
	float rayLength = cT + closestDistance;
	float volume = getVolumeWithFalloff(t + cT, rayLength);
	return (closestObject->type == SOUND)
		? getSoundEmitted(*closestObject, GetTime(), rayLength, s, lineToClosestObject, volume)
		: getWallBouncedSound(*closestObject, lineToClosestObject, objects, d, closestDistance, t, cT, numBounces, volume);
}

SoundInfo RayTracer::getWallBouncedSound(LineObject& wall,
	LineObject& rayToWall,
	LineBuffer& objects,
	Vec2& rayDirection,
	float lengthToWall,
	float remainingRayLength,
	float totalRayLength,
	int numBounces,
	float fallOff) {
	DrawLine(rayToWall.start.x, rayToWall.start.y, rayToWall.end.x, rayToWall.end.y, Color{ 40, 140, 250, (unsigned char)(fallOff * 255) });
	Vec2 normal = wall.normal;
	float dP = Vec2::dot(rayDirection, normal);
	if (dP > 0) {
		normal *= -1;
		dP *= -1;
	}
	Vec2 reflection = rayDirection - normal * (2 * dP);
	SoundInfo reflS = castRay(rayToWall.end, reflection, remainingRayLength - lengthToWall, totalRayLength, numBounces + 1, objects);
	SoundInfo refrS = castRay(rayToWall.end, rayDirection, remainingRayLength - lengthToWall, totalRayLength, numBounces + 1, objects);
	return reflS * wall.reflection + refrS * wall.refraction;
}

SoundInfo RayTracer::getSoundEmitted(LineObject& sound,
	float currentTime,
	float rayLength,
	Vec2& s,
	LineObject cL,
	float p) {
	DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, Color{ 200, 200, 100, (unsigned char)(p * 255) });
	EmittedSound* emittedSound;
	for (int i = 0; i < sound.numActive; i++) {
		emittedSound = &sound.activeSounds[i];
		if (cantHearSound(emittedSound->first, currentTime - emittedSound->second, rayLength / SOUND_SPEED)) continue;
		emittedSound->first.volume = p;
		DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, GREEN);
		return emittedSound->first;
	}
	return SoundInfo{};
}