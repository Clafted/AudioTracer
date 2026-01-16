#pragma once

#include "../include/TracerEngine.hpp"

TracerEngine::TracerEngine() : listener(lB, 0) {}
TracerEngine::TracerEngine(int numThreads) : listener(lB, numThreads){}

int TracerEngine::loadMap(const char* file) {
	if (vB.loadData(file) == -1) return -1;
	lB.loadData(vB.vertices, vB.endOfVertices);
	return 0;
}

void TracerEngine::listen() {
	for (LineObject* sound : soundSources) {
		sound->deleteOldSounds();
	}
	currentTime = GetTime();
	listener.listen(lB);
	deltaTime = currentTime - prevTime;
	prevTime = currentTime;
	listener.playDetectedSounds();
}

void TracerEngine::clearDetected() {
	listener.clearDetected();
}

int TracerEngine::addObject(LineObject object) {
	if (lB.lineCount >= MAX_LINE_COUNT) return -1;
	lB.lines[lB.lineCount] = object;
	if (object.type == SOUND) soundSources.push_back(&lB.lines[lB.lineCount]);
	return lB.lineCount++;
}