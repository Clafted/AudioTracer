#include "../include/SoundListener.hpp"
#include <iostream>

std::mutex SoundListener::lock;

void samplePixels(const LineBuffer& objects, 
	std::pair<Vec2, SoundInfo>* detPairs, 
	int start, 
	int end,
	Vec2& pos,
	int& sampleSize, 
	int& resolution);

SoundListener::SoundListener(LineBuffer& objects, int numThreads) : threadGrp(numThreads) {
	if (numThreads > resolution) resolution = numThreads;
	for (int i = 0; i < numThreads; i++) {
		threadGrp.assignTask(
			i,
			&samplePixels,
			std::ref(objects),
			std::ref(detPairs),
			i * resolution / numThreads, 
			std::min(resolution, (i+1) * resolution / numThreads),
			std::ref(pos),
			std::ref(sampleSize),
			std::ref(resolution));
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

void samplePixels(const LineBuffer& objects,
	std::pair<Vec2, SoundInfo>* detPairs, 
	int start, 
	int end,
	Vec2& pos,
	int& sampleSize, 
	int& resolution) {

	Vec2 direction, startPos;
	SoundInfo sound;
	float theta;

	SoundListener::lock.lock();
	Vec2 cur_pos = pos;
	int cur_sampleSize = sampleSize;
	int cur_resolution = resolution;
	SoundListener::lock.unlock();

	int spp = cur_sampleSize / cur_resolution;
	for (int pxl = start; pxl < end; pxl++) {
		theta = 6.28f * pxl / cur_resolution;
		detPairs[pxl] = { Vec2{cos(theta), sin(theta)}, SoundInfo()};
		for (int i = pxl*spp; (i < (pxl+1)*spp) || (pxl+1 == cur_resolution && i < cur_sampleSize); i++) {
			theta = 6.28f * i / cur_sampleSize;
			direction = Vec2{ cos(theta), sin(theta) };
			startPos = cur_pos + direction * 10.0f;
			detPairs[pxl].second += RayTracer::castRay(startPos, direction, 3000.0f, 0.0f, 0, objects);
		}
		if (pxl + 1 != cur_resolution) detPairs[pxl].second /= spp;
		else detPairs[pxl].second /= cur_sampleSize - (pxl*spp);
	}
}

void SoundListener::clearDetected()
{
	for (int i = 0; i < resolution; i++) {
		detPairs[i] = { 0, SoundInfo() };
	}
}