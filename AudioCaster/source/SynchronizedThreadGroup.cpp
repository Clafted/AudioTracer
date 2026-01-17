#pragma once

#include "../include/SynchronizedThreadGroup.hpp"

SynchronizedThreadGroup::SynchronizedThreadGroup(size_t numThreads) : numThreads(numThreads) {
	wait.store(true);
	destroyThreads.store(false);
	threads = new std::thread[numThreads];
	if (threads == nullptr) {
		throw std::runtime_error("FAILED TO ALLOCATE MEMORY FOR SYNC_THRD_GRP");
	}
	finishFlags = new std::atomic_bool[numThreads];
	if (finishFlags == nullptr) {
		throw std::runtime_error("FAILED TO ALLOCATE MEMORY FOR SYNC_THRD_GRP");
	}
}

SynchronizedThreadGroup::~SynchronizedThreadGroup() {
	destroyThreads.store(true);
	for (int i = 0; i < numThreads; i++) {
		if (threads[i].joinable()) threads[i].join();
	}
}

void SynchronizedThreadGroup::runGroup() {
	resetFlags();
	wait.store(false);
	waitForThreadsToFinish();
	wait.store(true);
}

void SynchronizedThreadGroup::resetFlags() {
	for (int i = 0; i < numThreads; i++) {
		finishFlags[i].store(false);
	}
}

void SynchronizedThreadGroup::waitForThreadsToFinish() const {
	int counter = 0, idx = 0;
	while (counter < numThreads) {
		if (true == finishFlags[idx].load()) counter++;
		else counter = 0;
		idx = (idx + 1) % numThreads;
	}
}