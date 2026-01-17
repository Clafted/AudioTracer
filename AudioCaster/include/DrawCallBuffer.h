#pragma once

#include <atomic>
#include "DrawCall.h"

#define MAX_CALLS_PER_FRAME 100000

class DrawCallBuffer {
	DrawCall calls[MAX_CALLS_PER_FRAME] = {};
	std::atomic_int count = 0, head = 0;

public:

	void addCall(const DrawCall& call) {
		if (count >= MAX_CALLS_PER_FRAME) return;
		calls[count++] = call;
	}


	bool isEmpty() {
		return count == 0;
	}

	bool hasNextCall() {
		return head < count;
	}
	
	int getCallCount() {
		return count;
	}

	DrawCall getNextCall() {
		if (!hasNextCall()) return DrawCall();
		return calls[head++];
	}

	void reset() {
		count = 0;
		head = 0;
	}
};