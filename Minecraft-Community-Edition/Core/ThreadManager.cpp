#include "ThreadManager.hpp"
#include "..\IO\Logger.hpp"
#define LOG_INFO(...) qBus.post(event::Log(event::Log::INFO, std::format(__VA_ARGS__)))
#define LOG_ERROR(...) qBus.post(event::Log(event::Log::ERROR, std::format(__VA_ARGS__)))
namespace mce::core {
	ThreadManager::ThreadManager(QEventBus& qBus) : qBus(qBus) {
		qBus.subscribe<event::ThreadFinished>([this](const auto& e) {
			onThreadFinished(e);
			});
		qBus.subscribe<event::ThreadStarted>([this, &qBus](const auto& e) {
			// Log thread name that started
			LOG_INFO("[ThreadManager] Thread '{}' started", e.threadName.data());
			std::format("", 10);
			});
	}
	ThreadManager::~ThreadManager() {
		ThreadManager::cleanupFinished();
	}
	void ThreadManager::launch(Thread* th) {
		std::lock_guard<std::mutex> lock(mutex);
		if (threads.contains(th)) {
			th->launch();
		}
	}

	void ThreadManager::launch(const eastl::string& name) {
		std::lock_guard<std::mutex> lock(mutex);
		auto it = eastl::find_if(threads.begin(), threads.end(),
			[&](const auto& pair) {
				return pair.first && pair.second.name == name;
			});

		if (it != threads.end() && it->first) {
			it->first->launch();
		}
		else {
			//Log the thread name could not be found
		}
	}

	void ThreadManager::waitAll() {
		std::lock_guard<std::mutex> lock(mutex);
		for (auto& [ptr, info] : threads) {
			ptr->wait();
		}
		cleanupFinishedUnlocked();
	}

	void ThreadManager::cleanupFinished() {
		std::lock_guard<std::mutex> lock(mutex);
		ThreadManager::cleanupFinishedUnlocked();
	}

	void ThreadManager::printStats() {
		std::lock_guard<std::mutex> lock(mutex);
		size_t running = 0, finished = 0;
		for (auto& [_, info] : threads) {
			info.finished ? ++finished : ++running;
		}

		// Debug the amount of threads that is finished as well how many is running
	}

	size_t ThreadManager::activeCount() const {
		std::lock_guard<std::mutex> lock(mutex);
		size_t count = 0;
		for (auto& [_, info] : threads) {
			if (!info.finished) ++count;
		}
		return count;
	}

	void ThreadManager::onThreadFinished(const event::ThreadFinished& e) {
		std::lock_guard<std::mutex> lock(mutex);
		auto it = threads.find(e.thread);
		
		if (it != threads.end()) {
			it->second.finished = true;
			it->second.endTime = std::chrono::steady_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(it->second.endTime - it->second.startTime).count() * 0.001;
			
			//Log the thread name and how long it took
			LOG_INFO("[ThreadManager] thread name: {}, exited and lifespan of it: {} seconds", it->second.name.data(), duration);
		}
		cleanupFinishedUnlocked();
	}

	void ThreadManager::cleanupFinishedUnlocked() {
		threadStorage.erase(
			eastl::remove_if(threadStorage.begin(), threadStorage.end(), 
				[](auto& t) {return t == nullptr; }),
			threadStorage.end());
	}




}
