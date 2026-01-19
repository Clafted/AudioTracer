#pragma once

#include "../include/SynchronizedThreadGroup.hpp";

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
	int counter = 0;
	auto start = std::chrono::high_resolution_clock::now();
	auto current = std::chrono::high_resolution_clock::now();
	float elapsedTime = std::chrono::duration<double, std::milli>(current-start).count();

	while (counter < numThreads) {
		if (true == finishFlags[counter].load()) counter++;

		current = std::chrono::high_resolution_clock::now();
		elapsedTime = std::chrono::duration<double, std::milli>(current - start).count();
		if (elapsedTime >= MAX_FRAME_TIME_MS) {
			std::string message = "\nWARNING: SYNCTHRDGRP TIMEOUT\n";
			for (int i = 0; i < numThreads; i++) {
				message += "\tTHREAD_" + std::to_string(i) + ": " + (finishFlags[i].load() ? "FINISHED\n" : "NOT FINISHED\n");
			}
			std::cerr << message;
			break;
		}
	}
}