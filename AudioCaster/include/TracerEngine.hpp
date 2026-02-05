#pragma once

#include <vector>

#include "SoundListener.hpp"
#include "VertexBuffer.hpp"
#include "LineBuffer.hpp"
#include "DrawCallBuffer.h"

class TracerEngine {

	int numFrames = 0;
	int totalRays = 0;
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();;

public:

	std::vector<LineObject*> soundSources = {};
	LineBuffer lB;

	VertexBuffer vB;
	DrawCallBuffer drawBuffer;
	SoundListener listener;
	float currentTime = 0.0f;
	float prevTime = 0.0f;
	float deltaTime = 0.0f;

	TracerEngine();
	TracerEngine(int numThreads);
	~TracerEngine();

	int loadMap(const char* path);
	int addObject(LineObject object);
	void listen();
	void clearDetected();

	inline float getFPS() {
		return 1.0f / deltaTime;
	}
};