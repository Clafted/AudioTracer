#pragma once

#include <atomic>
#include "DrawCall.h"

#define MAX_CALLS_PER_FRAME 10000

// Class to concurrently store raylib calls to DrawLine(). Thread-safe.
class DrawCallBuffer {

	DrawCall calls[MAX_CALLS_PER_FRAME] = {};
	std::atomic_int count = 0, head = 0;

public:

	void addCall(const DrawCall& call) {
		if (count.load() >= MAX_CALLS_PER_FRAME) return;
		calls[count++] = call;
	}


	inline bool isEmpty() {
		return count.load() == 0;
	}

	inline bool hasNextCall() {
		return head.load() < count.load();
	}
	
	inline int getCallCount() {
		return count.load();
	}

	DrawCall getNextCall() {
		if (!hasNextCall()) return DrawCall();
		return calls[head++];
	}

	void reset() {
		count.store(0);
		head.store(0);
	}
};