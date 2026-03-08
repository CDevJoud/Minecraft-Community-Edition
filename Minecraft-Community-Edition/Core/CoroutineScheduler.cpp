#include "CoroutineScheduler.hpp"

namespace mce{
	CoroutineScheduler::~CoroutineScheduler() {
		std::unique_lock lock(tasksMutex);
		CoroutineScheduler::vecTasks.clear();
		CoroutineScheduler::vecTasks.shrink_to_fit();
	}

	void CoroutineScheduler::addTask(Callback cb) {
		Task task;
		task.callback = cb;
		task.interval = cb();
		task.clock.restart();

		std::unique_lock lock(tasksMutex);
		CoroutineScheduler::vecTasks.push_back(task);
	}

	void CoroutineScheduler::processTasks() {
		std::unique_lock lock(tasksMutex);
		for (auto&[callback, interval, taskClock] : CoroutineScheduler::vecTasks) {
			if (taskClock.getElapsedTime() >= interval) {
				interval = callback();
				taskClock.restart();
			}
		}
	}
}
