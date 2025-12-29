#pragma once

#include "../include/SynchronizedThreadGroup.hpp"

SynchronizedThreadGroup::SynchronizedThreadGroup(int numThreads) : numThreads(numThreads) {
	wait.store(true);
	destroyThreads.store(false);
	threads = (std::thread*)calloc(numThreads, sizeof(std::thread));
	if (threads == nullptr) {
		throw std::runtime_error("FAILED TO ALLOCATE MEMORY FOR SYNC_THRD_GRP");
	}
	finishFlags = (std::atomic<bool>*)calloc(numThreads, sizeof(std::atomic<bool>));
	if (finishFlags == nullptr) {
		throw std::runtime_error("FAILED TO ALLOCATE MEMORY FOR SYNC_THRD_GRP");
	}
}

SynchronizedThreadGroup::~SynchronizedThreadGroup() {
	destroyThreads.store(true);
	for (int i = 0; i < numThreads; i++) {
		if (threads[i].joinable()) threads[i].join();
	}
	delete threads;
	delete finishFlags;
}

void SynchronizedThreadGroup::runGroup() {
	bool allFinished = false;
	wait.store(false);
	while (!allFinished) {
		allFinished = true;
		for (int i = 0; i < numThreads; i++) {
			if (finishFlags[i].load()) continue;
			allFinished = false;
			break;
		}
	}
	wait.store(true);
}