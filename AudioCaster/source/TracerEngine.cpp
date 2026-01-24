#pragma once

#include "../include/TracerEngine.hpp"

TracerEngine::TracerEngine() : listener(lB, drawBuffer, 0) {}
TracerEngine::TracerEngine(int numThreads) : listener(lB, drawBuffer, numThreads){}

TracerEngine::~TracerEngine() {
	auto end = std::chrono::high_resolution_clock::now();
	float elapsedTime = std::chrono::duration<double>(end - start).count();
	std::cout << "\n\n"
		<< "\nSTATISTICS:"
		<< "\nElapsed time(sec): " << elapsedTime
		<< "\nAvg FPS: " << numFrames / elapsedTime
		<< "\nAvg frame-time (ms): " << 1000.0f * elapsedTime / numFrames
		<< "\nAvg # of rays per frame: " << totalRays / numFrames
		<< "\n\n" << std::endl;
}

int TracerEngine::loadMap(const char* file) {
	if (-1 == vB.loadData(file)) return -1;
	lB.loadData(vB.vertices, vB.endOfVertices);
	return 0;
}

void TracerEngine::listen() {
	for (LineObject* sound : soundSources) {
		sound->deleteOldSounds();
	}
	currentTime = GetTime();
	listener.listen(lB);
	listener.playDetectedSounds();
	
	totalRays += listener.getNumRays();
	numFrames++;
	deltaTime = currentTime - prevTime;
	prevTime = currentTime;
}

void TracerEngine::clearDetected() {
	listener.clearDetected();
	drawBuffer.reset();
}

int TracerEngine::addObject(LineObject object) {
	if (lB.lineCount >= MAX_LINE_COUNT) return -1;
	lB.lines[lB.lineCount] = object;
	if (object.type == SOUND) soundSources.push_back(&lB.lines[lB.lineCount]);
	return lB.lineCount++;
}