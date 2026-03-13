#pragma once
#include "Thread.hpp"
#include <EASTL/unique_ptr.h>
#include <EASTL/string.h>
#include <SFML/System.hpp>
#include "IEvent.hpp"

/**
 * @file ThreadManager.hpp
 * @brief Manages lifecycle and bookkeeping for mce::Thread instances.
 *
 * ThreadManager provides thread creation helpers, launching, waiting and
 * statistics. It stores owned Thread instances and a map of Thread -> ThreadInfo
 * for monitoring start/end times and finished state.
 */

namespace mce {

	class ThreadManager {
	public:
		/**
		 * @brief Metadata stored per managed thread.
		 *
		 * ThreadInfo contains a pointer to the Thread instance (non-owning),
		 * a human-readable name, timestamps for start/end and a finished flag.
		 */
		struct ThreadInfo {
			/** @brief Non-owning pointer to the tracked Thread. */
			Thread* thread;
			/** @brief Human readable thread name. */
			eastl::string name;
			/** @brief Time point when the thread was started. */
			std::chrono::steady_clock::time_point startTime, endTime;
			/** @brief True when the thread has finished execution. */
			bool finished = false;
		};

		/**
		 * @brief Construct a ThreadManager bound to a QEventBus.
		 * @param qBus Reference to the application's QEventBus used to post thread events.
		 *
		 * The ThreadManager subscribes to thread lifecycle events on qBus to
		 * maintain accurate bookkeeping.
		 */
		explicit ThreadManager(QEventBus& qBus);

		/** @brief Destructor cleans up managed threads and subscriptions. */
		~ThreadManager();

		/**
		 * @brief Create (and store) a Thread from a generic callable.
		 * @tparam F Callable type (lambda, functor or function pointer).
		 * @param name Human readable name for the thread.
		 * @param func Callable to execute when the thread runs.
		 * @return Pointer to the created Thread (owned by ThreadManager).
		 *
		 * The returned Thread pointer is non-owning; ThreadManager retains ownership
		 * in its internal storage (threadStorage). The thread is created but not
		 * launched — call launch() to start it.
		 */
		template<typename F>
		Thread* createThread(const eastl::string& name, F func);

		/**
		 * @brief Create (and store) a Thread for a member function on an object.
		 * @tparam C Class type of the target object.
		 * @param name Human readable name for the thread.
		 * @param func Pointer to the member function to call.
		 * @param object Pointer to the object instance on which to call the member function.
		 * @return Pointer to the created Thread (owned by ThreadManager).
		 *
		 * Creates a Thread adapter that will call (object->*func)() when executed.
		 */
		template<typename C>
		Thread* createThread(const eastl::string& name, void(C::* func)(), C* object);

		/**
		 * @brief Launch a previously created Thread instance.
		 * @param th Pointer to the Thread returned from createThread().
		 *
		 * This will call Thread::launch() on the provided Thread pointer.
		 */
		void launch(Thread* th);

		/**
		 * @brief Convenience: launches the thread with the given name.
		 * @param name Name of the thread to launch.
		 *
		 * Finds the Thread associated with name and launches it. If multiple
		 * threads share the same name behavior depends on underlying container order.
		 */
		void launch(const eastl::string& name);

		/**
		 * @brief Block until all managed threads have finished executing.
		 *
		 * Joins/waits any active threads tracked by this manager.
		 */
		void waitAll();

		/**
		 * @brief Remove and cleanup finished threads from internal storage.
		 *
		 * This method may be used regularly to free resources held for threads
		 * that have already completed.
		 */
		void cleanupFinished();

		/**
		 * @brief Print statistics (e.g. runtime durations) for managed threads.
		 *
		 * Implementations should format and output info about active/finished
		 * threads using the project's logging facilities.
		 */
		void printStats();

		/**
		 * @brief Count of currently active (non-finished) threads.
		 * @return Number of active threads being tracked.
		 */
		size_t activeCount() const;

	private:

		/**
		 * @brief Event handler invoked when a ThreadFinished event is posted.
		 * @param e The ThreadFinished event describing which thread finished.
		 *
		 * Updates internal bookkeeping to mark the corresponding ThreadInfo
		 * as finished and record end time.
		 */
		void onThreadFinished(const event::ThreadFinished& e);

		/**
		 * @brief Internal helper to remove finished threads without locking.
		 *
		 * Assumes the caller already holds the manager mutex.
		 */
		void cleanupFinishedUnlocked();
	private:
		/** @brief Mutex protecting access to internal containers. */
		mutable std::mutex mutex;
		/** @brief Map from Thread pointer to ThreadInfo for monitoring. */
		eastl::unordered_map<Thread*, ThreadInfo> threads;
		/** @brief Storage owning created Thread instances. */
		eastl::vector<eastl::unique_ptr<Thread>> threadStorage;
		/** @brief Reference to the event bus used to post/start/finish events. */
		QEventBus& qBus;
	};
	template<typename F>
	inline Thread* ThreadManager::createThread(const eastl::string& name, F func) {
		auto thread = eastl::make_unique<Thread>(qBus, [this, func, name]() {
			qBus.post(event::ThreadStarted{ nullptr, name });
			func();
			});

		Thread* rawThread = thread.get();

		std::lock_guard<std::mutex> lock(mutex);
		ThreadInfo ti;
		ti.startTime = std::chrono::steady_clock::now();
		ti.name = name;
		ti.thread = rawThread;
		ti.endTime = {};
		ti.finished = false;
		threads.emplace(rawThread, ti);

		threadStorage.push_back(eastl::move(thread));
		return rawThread;
	}
	template<typename C>
	inline Thread* ThreadManager::createThread(const eastl::string& name, void(C::* func)(), C* object) {
		qBus.post(event::ThreadStarted{ nullptr, name });
		auto thread = eastl::make_unique<Thread>(
			qBus,
			func,
			object
		);

		Thread* rawThread = thread.get();
		{
			std::lock_guard<std::mutex> lock(mutex);
			threads.emplace(rawThread, ThreadInfo{
				thread,
				name,
				std::chrono::steady_clock::now(),
				{},
				false
				});
		}

		threadStorage.push_back(eastl::move(thread));
		return rawThread;
	}
}
