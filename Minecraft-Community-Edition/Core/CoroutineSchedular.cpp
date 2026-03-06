#include "CoroutineSchedular.hpp"

namespace mce{
	void CoroutineSchedular::processCoroutineSchedular() {
		for (auto& task : CoroutineSchedular::vecTasks) {
			if (task.clock.getElapsedTime() >= task.interval) {
				task.interval = task.callback();
				task.clock.restart();
			}
		}
	}
}
