#include "CoroutineSchedular.hpp"

namespace mce{
	CoroutineSchedular::~CoroutineSchedular() {
		CoroutineSchedular::vecTasks.clear();
		CoroutineSchedular::vecTasks.shrink_to_fit();
	}
	void CoroutineSchedular::processCoroutineSchedular() {
		for (auto& task : CoroutineSchedular::vecTasks) {
			if (task.clock.getElapsedTime() >= task.interval) {
				task.interval = task.callback();
				task.clock.restart();
			}
		}
	}
}
