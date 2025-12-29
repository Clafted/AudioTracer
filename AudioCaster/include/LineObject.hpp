#ifndef LINE_H
#define LINE_H

#include "Vec2.h"
#include "SoundInfo.hpp"
#include <raylib.h>
#include <cmath>
#include <string>
#include <utility>

#define MAX_SOUND_COUNT 20
#define MAX_SOUND_LENGTH 1.0
#define SOUND_SPEED 1043.0f

enum OBJECT_TYPE { WALL = 0, SOUND = 1 };

struct LineObject
{
	Vec2 start, end;
	Vec2 normal;
	
	std::pair <SoundInfo, float> activeSounds[MAX_SOUND_COUNT] = {};
	std::string soundFile;
	float absorption = 0.0f;
	float reflection = 0.5f;
	float refraction = 0.3f;
	float length = 0.0f;
	int numActive = 0;
	int radius = 40;

	OBJECT_TYPE type = WALL;

	LineObject() : type(WALL) {}
	LineObject(Vec2 start, Vec2 end, OBJECT_TYPE type = WALL) : start(start), end(end), type(type) {
		normal = Vec2{ end.y - start.y, start.x - end.x };
		normal.normalize();
		length = getLength(start, end);
	}
	LineObject(Vec2 start, int radius, const char* soundFile) : start(start), end(0), radius(radius), type(SOUND)
	{
		this->soundFile = soundFile;
		reflection = 0.6f;
		refraction = 0.4f;
		absorption = 1 - reflection - refraction;
	}

	void addSound(const char* soundFile)
	{
		this->soundFile = soundFile;
	}

	void playSound() {
		if (numActive == 10) return;
		activeSounds[numActive] = { SoundInfo{soundFile, 1.0f},  GetTime()};
		numActive++;
	}

	/**
	* Replace all inactive sounds with remaining active sounds.
	* Maintains contiguity of activeSounds (i.e., all sounds from 0-numActive are active)
	*/
	void deleteOldSounds()
	{
		int firstEmpty = -1;
		int currentActive = numActive;
		float elapsedTime;
		for (int i = 0; i < currentActive; i++) {
			elapsedTime = GetTime() - activeSounds[i].second;
			if (firstEmpty == -1								// No empty spot
				&& (activeSounds[i].first.volume <= 0.0f		// No sound
				|| elapsedTime > MAX_SOUND_LENGTH))	{		// Inactive
				firstEmpty = i;
				numActive--;
			} 
			else if (firstEmpty != -1							// Has empty spot
					&& activeSounds[i].first.volume > 0.0f		// Has sound
					&& elapsedTime <= MAX_SOUND_LENGTH)	{		// Active
				activeSounds[firstEmpty] = activeSounds[i];
				firstEmpty = i;
			}
		}
	}

	float getSlope() {	
		return (end.y - start.y) / (end.x - start.x); 
	}

	float getNormal() {
		return -(end.x - start.x) / (end.y - start.y);
	}
	 
	static float getLength(const Vec2& p1, const Vec2& p2) {
		return sqrt( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
	}
	
	bool containsPoint(const Vec2 &p) const {
		return getLength(start, p) + getLength(p, end) - length < 0.5f;
	}


	void move(const Vec2 displacement){
		start += displacement;
		end += displacement;
	}
};

#endif 