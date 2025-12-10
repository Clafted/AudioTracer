#pragma once

#include <vector>

#include "SoundListener.hpp"
#include "VertexBuffer.hpp"
#include "LineBuffer.hpp"

class TracerEngine {
	
public:

	std::vector<LineObject*> soundSources;
	LineBuffer lB;
	VertexBuffer vB;
	SoundListener listener;
	float currentTime, prevTime;

	int loadMap(const char* path);
	void listen();
	void clearDetected();
	int addObject(LineObject object);
};