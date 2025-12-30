#include "../include/SoundListener.hpp"
#include <iostream>

void samplePixels(
	SoundListener& config,
	LineBuffer& objects,
	std::pair<Vec2, SoundInfo>* dest,
	int start, int end);

SoundListener::SoundListener(LineBuffer& objects, int numThreads) : threadGrp(numThreads) {
	if (numThreads > resolution) resolution = numThreads;
	for (int i = 0; i < numThreads; i++) {
		threadGrp.assignTask(
			i,
			&samplePixels,
			std::ref(*this),
			std::ref(objects),
			std::ref(detPairs),
			i * resolution / numThreads,
			std::min(resolution, (i + 1) * resolution / numThreads));
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
	std::string cFile = "";
	SoundInfo* sound;
	Vec2* soundIncomingDirection;
	Sound s{};
	for (int i = 0; i < resolution; i++) {
		sound = &detPairs[i].second;
		soundIncomingDirection = &detPairs[i].first;
		if (sound->empty()) continue;
		// Load new sounds.
		if (cFile != sound->file && loadedSounds.find(sound->file) == loadedSounds.end()) {
			loadedSounds[sound->file] = LoadSound(sound->file.c_str());
			cFile = sound->file;
		}
		s = LoadSoundAlias(loadedSounds[sound->file]);
		SetSoundVolume(s, sound->volume);
		SetSoundPan(s, -0.5f * (soundIncomingDirection->x - 1.0f));
 		PlaySound(s);
		DrawLine(pos.x, pos.y, pos.x + soundIncomingDirection->x * sound->volume * 5000.0f, pos.y + soundIncomingDirection->y * sound->volume * 5000.0f, PINK);
	}
	numDetected = 0;
}

void SoundListener::listen(LineBuffer& objects)
{
	if (sampleSize / resolution == 0) return;
	RayTracer::resetTracer();
	threadGrp.runGroup();
}

void samplePixels(SoundListener& config,
	LineBuffer& objects,
	std::pair<Vec2, SoundInfo>* dest,
	int start, 
	int end) {

	Vec2 direction, startPos;
	SoundInfo sound;
	float theta;

	config.lock.lock();
	Vec2 cur_pos = config.getPosition();
	int cur_sampleSize = config.getSampleSize();
	int cur_resolution = config.getResolution();
	config.lock.unlock();

	int spp = cur_sampleSize / cur_resolution;
	for (int pxl = start; pxl < end; pxl++) {
		theta = 6.28f * pxl / cur_resolution;
		dest[pxl] = { Vec2{cos(theta), sin(theta)}, SoundInfo()};
		for (int i = pxl*spp; (i < (pxl+1)*spp) || (pxl+1 == cur_resolution && i < cur_sampleSize); i++) {
			theta = 6.28f * i / cur_sampleSize;
			direction = Vec2{ cos(theta), sin(theta) };
			startPos = cur_pos + direction * 10.0f;
			dest[pxl].second += RayTracer::castRay(startPos, direction, 3000.0f, 0.0f, 0, objects);
		}
		if (pxl + 1 != cur_resolution) dest[pxl].second /= spp;
		else dest[pxl].second /= cur_sampleSize - (pxl*spp);
	}
}

void SoundListener::clearDetected()
{
	for (int i = 0; i < resolution; i++) {
		detPairs[i] = { 0, SoundInfo() };
	}
}