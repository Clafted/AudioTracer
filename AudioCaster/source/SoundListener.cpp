#include "../include/SoundListener.hpp"
#include <iostream>

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

void SoundListener::listen(LineBuffer& objects, float dTime)
{
	SoundInfo s;
	Vec2 d, p;
	SoundInfo avg;
	float theta;
	int spp = sampleSize/resolution;
	int r;
	rayTracer.resetTracer();
	if (spp == 0) return;

 	for (int i = 0; i < sampleSize; i++) 
	{
		// Accumulate rays
		theta = 6.28f * i / sampleSize;
		d = Vec2{ cos(theta), sin(theta) };
		p = pos + d * 10.0f;
		s = rayTracer.castRay(p, d, 3000.0f, 0.0f, 0, objects);
		avg += s;
		if ((i + 1) % spp != 0 || avg.empty()) continue;
		// Average rays into one SoundInfo obj
		avg /= spp;
		r = i / spp;
		theta = 6.28f * r / resolution;
		detPairs[r] = { Vec2{ cos(theta), sin(theta) }, avg };
		avg = { "", 0.0f };
	}
}

void SoundListener::clearDetected()
{
	for (int i = 0; i < resolution; i++) {
		detPairs[i] = { 0, SoundInfo() };
	}
}