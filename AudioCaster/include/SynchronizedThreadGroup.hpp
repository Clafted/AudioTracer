#pragma once

#include <thread>
#include <iostream>
#include <atomic>
#include <string>
#include <chrono>

#define MAX_FRAME_TIME_MS 500.0

// Class to manage a group of synchronized threads
class SynchronizedThreadGroup {
	
	const size_t numThreads = 0;
	std::atomic_bool wait, destroyThreads;
	std::atomic_bool* finishFlags = nullptr;
	std::thread* threads = nullptr;
	
	void resetFlags();
	void waitForThreadsToFinish() const;

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

	template <class F, class... Args>
	static void threadFunction(
		SynchronizedThreadGroup& manager,
		std::atomic_bool& flag,
		F&& f,
		Args&&... args) {

		std::cout << std::string("CREATED THREAD_" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "\n");
		while (!manager.shouldDestroyThreads()) {
			while (manager.threadsShouldWaitForStart());
			f(args...);
			flag.store(true);
			while (manager.threadsShouldWaitForFinish());
		}
		std::cout << std::string("EXITING THREAD_" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "\n");
	}

public:

	SynchronizedThreadGroup(size_t numThreads);
	~SynchronizedThreadGroup();
	
	inline size_t getNumThreads() {
		return numThreads;
	}
	
	/**
	* Runs synchronized threads. Threads that do not complete
	* their tasks within MAX_FRAME_TIME_MS milliseconds will
	* be skipped for this run.
	*/
	void runGroup();

	/**
	* Assign a function f with arguments args to thread tarrget.
	* 
	* @param target index of the thread to assign the task to
	* @param f function to be executed by the thread
	* @param args arguments to be passed to f
	* @returns true if the task was assigned successfully, false otherwise */
	template <class F, class... Args>
	bool assignTask(int target, F&& f, Args&&... args) {
		if (threads[target].joinable()) threads[target].join();
		threads[target] = std::thread(
			&threadFunction<F, Args...>, 
			std::ref(*this), 
			std::ref(finishFlags[target]), 
			f, args...);
		return true;
	}
};