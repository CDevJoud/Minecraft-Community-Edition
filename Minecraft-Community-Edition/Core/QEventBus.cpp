#include "QEventBus.hpp"

namespace mce {
	void QEventBus::process() {
		eastl::queue<eastl::unique_ptr<IEvent>> localQueue;
		{
			std::lock_guard<std::mutex> lock(QEventBus::queueMutex);
			eastl::swap(localQueue, QEventBus::queue);
		}
		this;
		while (!localQueue.empty()) {
			QEventBus::dispatch(*localQueue.front());
			localQueue.pop();
		}
	}

	void QEventBus::runAsync() {
		QEventBus::bIsRunning = true;
		QEventBus::thread = std::thread([this] {
			while (QEventBus::bIsRunning.load(std::memory_order_acquire)) {
				eastl::unique_ptr<IEvent> event;
				{
					std::unique_lock<std::mutex> lock(QEventBus::queueMutex);
					QEventBus::conditionalVariable.wait(lock, [&]() { return !QEventBus::queue.empty() || !QEventBus::bIsRunning.load(std::memory_order_acquire); });
					if (!QEventBus::bIsRunning.load(std::memory_order_acquire)) break;
					event = std::move(QEventBus::queue.front());
					QEventBus::queue.pop();
				}
				QEventBus::dispatch(*event);
			}
			});
	}

	void QEventBus::stop() {
		QEventBus::bIsRunning.store(false, std::memory_order_release);
		QEventBus::conditionalVariable.notify_all();
		if (QEventBus::thread.joinable()) QEventBus::thread.join();
	}

	QEventBus::~QEventBus() {
		QEventBus::stop();
	}

	void QEventBus::dispatch(const IEvent& e) {
		std::lock_guard<std::mutex> lock(QEventBus::mutex);
		auto it = QEventBus::handlers.find(typeid(e));
		if (it != QEventBus::handlers.end()) {
			for (auto& handler : it->second) {
				handler(e);
			}
		}
	}
}
