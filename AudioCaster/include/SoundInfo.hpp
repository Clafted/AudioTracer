#pragma once

#include <string>

struct SoundInfo {
	std::string file = "";
	float volume = 0.0f;

	SoundInfo() {}

	SoundInfo(std::string file, float volume) {
		this->file.assign(file);
		this->volume = volume;
	}

	inline bool empty() {
		return file == "" || file.empty() || volume <= 0.0f;
	}

	void clear() {
		file.clear();
		volume = 0.0f;
	}

	inline void operator=(const SoundInfo other) {
		file.assign(other.file.c_str());
		volume = other.volume;
	}

	inline SoundInfo operator+(const SoundInfo other) {
		return SoundInfo((file.empty()) ? other.file : file, volume + other.volume);
	}

	inline void operator+=(const SoundInfo other) {
		*this = *this + other;
	}

	inline SoundInfo operator*(const float a) {
		return SoundInfo( file, volume * a );
	}

	inline void operator*=(const float a) {
		*this = *this * a;
	}

	inline SoundInfo operator/(const float a) {
		return SoundInfo( file, volume / a );
	}

	inline void operator/=(const float a) {
		*this = *this / a;
	}
};