#pragma once

#include <vector>

#include "SoundListener.hpp"
#include "VertexBuffer.hpp"
#include "LineBuffer.hpp"

class TracerEngine {

public:

	std::vector<LineObject*> soundSources = {};
	LineBuffer lB;
	VertexBuffer vB;
	SoundListener listener;
	float currentTime = 0.0f, prevTime = 0.0f, deltaTime = 0.0f;

	TracerEngine();
	TracerEngine(int numThreads);

	int loadMap(const char* path);
	void listen();
	void clearDetected();
	int addObject(LineObject object);

	inline float getFPS() {
		return 1.0f / deltaTime;
	}
};