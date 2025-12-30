#pragma once

#include <thread>
#include <iostream>
#include <atomic>

class SynchronizedThreadGroup {
	
	const int numThreads;
	std::atomic<bool>* finishFlags;
	std::thread* threads;

	std::atomic<bool> wait, destroyThreads;

public:

	SynchronizedThreadGroup(int numThreads);

	~SynchronizedThreadGroup();
	
	inline int getNumThreads() {
		return numThreads;
	}

	inline bool shouldWait() {
		return wait.load();
	}

	inline bool shouldDestroyThreads() {
		return destroyThreads.load();
	}

	inline bool threadsShouldWaitForFinish() {
		return !wait.load() && !destroyThreads.load();
	}

	inline bool threadsShouldWaitForStart() {
		return wait.load() && !destroyThreads.load();
	}

	void runGroup();
	
	template <class F, class... Args>
	void assignTask(int target, F&& f, Args&&... args) {
		if (threads[target].joinable()) threads[target].join();
		threads[target] = std::thread(
			[] (SynchronizedThreadGroup& manager, std::atomic<bool>& flag, F&& f, Args&&... args) {
				std::cout << "CREATED THREAD_" << std::this_thread::get_id() << "\n";
				while (!manager.shouldDestroyThreads()) {
					while (manager.threadsShouldWaitForStart());
					flag.store(false);
					f(args...);
					flag.store(true);
					while (manager.threadsShouldWaitForFinish());
				}
				std::cout << "EXITING THREAD_" << std::this_thread::get_id() << "\n";
			}, 
			std::ref(*this), std::ref(finishFlags[target]), f, args...);
	}
};