#pragma once

#include <thread>
#include <iostream>
#include <atomic>

class SynchronizedThreadGroup {
	
	const size_t numThreads = 0;
	std::atomic_bool* finishFlags = nullptr;
	std::thread* threads = nullptr;

	std::atomic_bool wait, destroyThreads;

	void resetFlags();
	void waitForThreadsToFinish();

public:

	SynchronizedThreadGroup(size_t numThreads);

	~SynchronizedThreadGroup();
	
	inline size_t getNumThreads() {
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
			[] (SynchronizedThreadGroup& manager, 
				std::atomic_bool& flag, 
				F&& f,
				Args&&... args) {

				std::cout << "CREATED THREAD_" << std::this_thread::get_id() << "\n";
				while (!manager.shouldDestroyThreads()) {
					while (manager.threadsShouldWaitForStart());
					f(args...);
					flag.store(true);
					while (manager.threadsShouldWaitForFinish());
				}
				std::cout << "EXITING THREAD_" << std::this_thread::get_id() << "\n";

			}, 
			std::ref(*this), 
			std::ref(finishFlags[target]), 
			f, 
			args...
		);
	}
};