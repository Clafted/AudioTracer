#pragma once

#include <thread>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <chrono>

class SynchronizedThreadGroup {
	
	int numThreads;
	bool wait, destroyThreads;
	std::thread* threads;

public:

	std::mutex finishedMtx;
	int finishedCounter;

	SynchronizedThreadGroup(int numThreads) : numThreads(numThreads) {
		finishedCounter = 0;
		wait = true;
		destroyThreads = false;
		threads = (std::thread*)calloc(numThreads, sizeof(std::thread));
		if (threads == nullptr) {
			throw std::runtime_error("FAILED TO ALLOCATE MEMORY FOR SYNC_THRD_GRP");
		}
	}

	~SynchronizedThreadGroup() {
		destroyThreads = true;
		for (int i = 0; i < numThreads; i++) {
			threads[i].join();
		}
		delete threads;
	}
	
	inline int getNumThreads() {
		return numThreads;
	}

	inline int getNumFinished() {
		return finishedCounter;
	}

	inline bool shouldWait() {
		return wait;
	}

	inline bool shouldDestroyThreads() {
		return destroyThreads;
	}

	inline void runGroup() {
		finishedMtx.lock();
		finishedCounter = 0;
		finishedMtx.unlock();
		wait = false;
		while (true) {
			finishedMtx.lock();
			if (finishedCounter >= numThreads) {
				finishedMtx.unlock();
				break;
			}
			finishedMtx.unlock();
		};
		wait = true;
	}


	template <class F, class... Args>
	void assignTask(int target, F&& f, Args&&... args) {
		threads[target] = std::thread([](SynchronizedThreadGroup& manager, F&& f, Args&&... args) {
			while (!manager.shouldDestroyThreads()) {
				if (manager.shouldWait()) continue;
				f(args...);
				manager.finishedMtx.lock();
				manager.finishedCounter++;
				manager.finishedMtx.unlock();
				while (!manager.shouldWait() && !manager.shouldDestroyThreads()) {};
			}
		}, std::ref(*this), f, args...);
	}
};