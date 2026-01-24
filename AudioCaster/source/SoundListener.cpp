#include "../include/SoundListener.hpp"
#include <iostream>


void samplePixels(
	SoundListener& config,
	LineBuffer& objects,
	DrawCallBuffer& drawBuffer,
	std::pair<Vec2, SoundInfo>* dest,
	int start, 
	int end);


SoundListener::SoundListener(LineBuffer& objects, DrawCallBuffer& drawBuffer, int numThreads) : threadGrp(numThreads), detPairs() {
	
	clearDetected();
	resolution = numThreads;
	resolution = 20;
	sampleSize = 140;

	int pixelsPerThread = (int)ceil(resolution / numThreads);
	for (int i = 0; i < numThreads; i++) {
		threadGrp.assignTask(
			i,
			&samplePixels,
			std::ref(*this),
			std::ref(objects),
			std::ref(drawBuffer),
			std::ref(detPairs),
			i*pixelsPerThread, 
			((i+1)*pixelsPerThread >= resolution) 
			? resolution
			: (i + 1) * pixelsPerThread);
	}
}

SoundListener::~SoundListener() {
	clearDetected();
	for (auto i = loadedSounds.begin(); i != loadedSounds.end(); i++) {
		UnloadSound(i->second);
	}
	loadedSounds.clear();
}
 
void SoundListener::playDetectedSounds() {
	SoundInfo* sound;
	Vec2* soundIncomingDirection;
	Sound s;
	bool soundIsLoaded;
	for (int i = 0; i < resolution; i++) {
		soundIncomingDirection = &detPairs[i].first;
		sound = &detPairs[i].second;
		if (sound->empty()) continue;
		// Load new sounds
		
		soundIsLoaded = loadedSounds.find(sound->file) != loadedSounds.end();
		if (!soundIsLoaded) {
			Sound newSound = LoadSound(sound->file.c_str());
			if (!IsSoundValid(newSound)) break;
			loadedSounds.emplace(sound->file, newSound);
			soundIsLoaded = true;
		}
		if (!soundIsLoaded) continue;
		try {
			s = LoadSoundAlias(loadedSounds.at(sound->file)); // Can throw exception
			SetSoundVolume(s, sound->volume);
			SetSoundPan(s, -0.5f * (soundIncomingDirection->x - 1.0f));
			PlaySound(s);
			DrawLine(pos.x, pos.y, pos.x + soundIncomingDirection->x * sound->volume * 5000.0f, pos.y + soundIncomingDirection->y * sound->volume * 5000.0f, PINK);
		}
		catch (...) {
			std::cerr << "FAILED TO LOAD SOUND ALIAS: " << sound->file << std::endl;
		}
	}
	numDetected = 0;
}

void SoundListener::listen(LineBuffer& objects)
{
	if (sampleSize / resolution == 0) return;
	RayTracer::resetTracer();
	threadGrp.runGroup();
}

void accumulateSamples(
	unsigned int start,
	unsigned int end,
	unsigned int maxSamples,
	Vec2 pos,
	LineBuffer& objects,
	DrawCallBuffer& drawBuffer,
	SoundInfo& dest) {

	Vec2 direction, castPos;
	float theta;

	for (int i = start; i < end; i++) {
		theta = 6.28f * i / maxSamples;
		direction = Vec2(cos(theta), sin(theta));
		castPos = pos + direction * 10.0f;
		dest += RayTracer::castRay(castPos, direction, 3000.0f, 0.0f, 0, objects, drawBuffer);
	}

}

void samplePixels(
	SoundListener& config,
	LineBuffer& objects,
	DrawCallBuffer& drawBuffer,
	std::pair<Vec2, SoundInfo>* dest,
	int start, 
	int end) {

	Vec2 direction;
	SoundInfo sound;
	float theta;
	int spp, startSample, endSample;

	config.lock.lock();
	Vec2 cur_pos = config.getPosition();
	int cur_sampleSize = config.getSampleSize();
	int cur_resolution = config.getResolution();
	config.lock.unlock();

	spp = cur_sampleSize / cur_resolution;
	for (int pxl = start; pxl < end; pxl++) {
		theta = 6.28f * pxl / cur_resolution;
		dest[pxl] = std::make_pair(Vec2(cos(theta), sin(theta)), SoundInfo());

		startSample = pxl * spp;
		endSample = (pxl + 1) * spp;
		if (cur_sampleSize - endSample <= spp) endSample = cur_sampleSize;
		
		accumulateSamples(
			startSample,
			endSample, 
			cur_sampleSize,
			cur_pos, 
			objects,
			drawBuffer,
			dest[pxl].second);

		float divisor = spp; 
		if (endSample == cur_sampleSize) {
			divisor = cur_sampleSize - startSample;
		}
		dest[pxl].second /= divisor;
	}
}

void SoundListener::clearDetected() {
	for (int i = 0; i < resolution; i++) {
		detPairs[i].first = Vec2(0.0f);
		detPairs[i].second.clear();
	}
}