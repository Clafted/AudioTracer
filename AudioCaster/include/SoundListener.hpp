#ifndef RAYLISTENER_H
#define RAYLISTENER_H

#include <unordered_map>
#include <utility>
#include <mutex>
#include "Vec2.h"
#include "RayTracer.h"
#include "SynchronizedThreadGroup.hpp"

#define MAX_DETECTED 250	// miniaudio supports up to 254 channels
#define THREAD_COUNT 8

class SoundListener {
private:

	std::pair<Vec2, SoundInfo> detPairs[MAX_DETECTED] = {};
	std::unordered_map<std::string, Sound> loadedSounds = {};
	SynchronizedThreadGroup threadGrp;
	Vec2 pos = { 0, 0 };
	int sampleSize = 25;
	float dTime = 0.0f;
	int resolution = 0;
	int numDetected = 0;

public:

	std::mutex lock;
	
	SoundListener(LineBuffer& objects, int numThreads);

	~SoundListener();

	/* Plays all the sounds detected by castRay(). */
	void playDetectedSounds();

	/* Clears buffer of detected sounds. */
	void clearDetected();

	/* Listens for sounds from a buffer of objects
	* objects, with elapsed time dTime. */
	void listen(LineBuffer& objects);



	/* INLINE FUNCIONS */

	inline void incrementSampleSize(int increment) {
		if (10 < sampleSize + increment) sampleSize += increment;
	}

	inline void incrementMaxBounces(int increment) {
		if (0 < RayTracer::maxBounces + increment) RayTracer::maxBounces += increment;
	}

	inline void incrementResolution(int increment) {
		if (threadGrp.getNumThreads() <= resolution + increment && resolution + increment < MAX_DETECTED) resolution += increment;
	}

	inline void setPosition(Vec2 position) {
		pos = position;
	}

	inline const Vec2 getPosition() {
		return pos;
	}

	inline const std::pair<Vec2, SoundInfo>* getDetectedPairs() {
		return detPairs;
	}

	inline int getSampleSize() {
		return sampleSize;
	}

	inline int getMaxBounces() {
		return RayTracer::maxBounces;
	}

	inline int getNumRays() {
		return RayTracer::numRays.load();
	}

	inline int getResolution() {
		return resolution;
	}
};

#endif