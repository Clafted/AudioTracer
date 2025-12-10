#pragma once

#include <string>

struct SoundInfo
{
	std::string file = "";
	float volume = 0.0f;

	inline bool empty() {
		return file.empty() || volume <= 0.0f;
	}

	inline SoundInfo operator+(const SoundInfo other) {
		return SoundInfo{ (file.empty()) ? other.file : file, volume + other.volume };
	}

	inline void operator+=(const SoundInfo other) {
		*this = *this + other;
	}

	inline SoundInfo operator*(const float a) {
		return SoundInfo{ file, volume * a };
	}

	inline void operator*=(const float a) {
		*this = *this * a;
	}

	inline SoundInfo operator/(const float a) {
		return SoundInfo{ file, volume / a };
	}

	inline void operator/=(const float a) {
		*this = *this / a;
	}
};