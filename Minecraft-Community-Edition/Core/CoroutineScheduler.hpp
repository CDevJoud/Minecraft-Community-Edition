#pragma once
#include <mutex>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <EASTL/functional.h>
#include <EASTL/vector.h>

/**
 * @file CoroutineScheduler.hpp
 * @brief Simple coroutine scheduler that runs periodic callbacks using SFML time utilities.
 */

namespace mce {
/**
 * @brief Coroutine scheduler that manages periodic tasks.
 *
 * The scheduler stores tasks that each have their own clock and interval. Each
 * task's callback should return an sf::Time representing the time to wait
 * until the next invocation. When a task's clock has elapsed at least its
 * interval the callback is invoked and the returned sf::Time is used as the
 * new interval.
 * 
 * example usage:
 * 
 * addTask([]() -> sf::Time { 
 * 
 * return sf::seconds(1);
 * 
 * }
 */
	class CoroutineScheduler {
	public:
		/**
		 * @brief Callback type invoked by scheduled tasks.
		 *
		 * @returns An sf::Time that
		 * indicates how long to wait until the next invocation of this callback.
		 */
		using Callback = eastl::function<sf::Time()>;

		/**
		 * @brief Default constructor.
		 */
		CoroutineScheduler() = default;
		~CoroutineScheduler();

		/**
		 * @brief Represents a single scheduled task.
		 *
		 * Holds the callback to invoke, the next execution interval, and a clock
		 * used to measure elapsed time since the last invocation.
		 */
		struct Task {
			Callback callback; /**< Task callback invoked when interval elapses. */
			sf::Time interval; /**< Time interval until the next invocation. */
			sf::Clock clock;   /**< Clock used to measure elapsed time for this task. */
		};

		/**
		 * @brief Add a task to the scheduler.
		 * @param cb The callback to schedule. The callback is invoked immediately to obtain the initial interval.
		 *
		 * The callback should return an sf::Time specifying how long to wait until
		 * the next invocation. The task's clock is restarted when added.
		 */
		void addTask(Callback cb);

		/**
		 * @brief Process all scheduled tasks.
		 *
		 * This should be called periodically (for example once per frame). For
		 * each task if its clock has elapsed at least its interval, the task's
		 * callback is invoked, the returned sf::Time is used as the new interval,
		 * and the task clock is restarted.
		 */
		void processTasks();
	private:
		std::mutex tasksMutex;
		eastl::vector<Task> vecTasks; /**< Container of scheduled tasks. */

		sf::Clock clock; /**< Global clock (reserved for future use). */
	};
}

