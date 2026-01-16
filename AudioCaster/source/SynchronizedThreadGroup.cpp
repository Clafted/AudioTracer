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
	bool allFinished = false;
	int counter = 0;
	wait.store(false);
	while (counter < numThreads) {
		if (true == finishFlags[counter].load()) counter++;
		else counter = 0;
	}
	wait.store(true);
}