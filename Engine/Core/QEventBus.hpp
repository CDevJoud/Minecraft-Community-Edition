#pragma once
#include <EASTL/functional.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/queue.h>
#include <EASTL/unique_ptr.h>
#include <typeindex>
#include <mutex>
#include "IEvent.hpp"

/**
 * @file QEventBus.hpp
 * @brief Thread-safe event bus.
 */

namespace dms::core {

	/**
	 * @brief Token object that controls the lifetime of an event subscription.
	 *
	 * The destructor will call the stored unsubscribe callable if it is set,
	 * causing the associated handler to be removed from the event bus.
	 *
	 * This type is move- and copy-assignable depending on the semantics of the
	 * stored eastl::function. Use subscribeRAII to obtain a token.
	 */
	class SubscriptionToken {
	public:

		/**
		 * @brief Callable invoked to perform the unsubscribe action.
		 *
		 * If empty, destruction of the token has no effect.
		 */
		eastl::function<void()> unsubscribe;

		SubscriptionToken() = default;
		explicit SubscriptionToken(eastl::function<void()> func) : unsubscribe(eastl::move(func)) {}

		//disable copying
		SubscriptionToken(const SubscriptionToken&) = delete;
		SubscriptionToken& operator=(const SubscriptionToken&) = delete;

		//Allow moving
		SubscriptionToken(SubscriptionToken&& other) noexcept : unsubscribe(eastl::move(other.unsubscribe)) {
			other.unsubscribe = nullptr;
		}

		SubscriptionToken& operator=(SubscriptionToken&& other) noexcept {
			if (this != &other) {
				if (unsubscribe)
					unsubscribe();

				unsubscribe = eastl::move(other.unsubscribe);
				other.unsubscribe = nullptr;
			}
			return *this;
		}

		/**
		 * @brief Destructor.
		 *
		 * Automatically unsubscribes if an unsubscribe callable is present.
		 */
		~SubscriptionToken() { 
			if (unsubscribe) {
				unsubscribe();
				unsubscribe = nullptr;
			}
		}

		//Optional Explicit unsubscribe
		void reset() {
			if (unsubscribe) {
				unsubscribe();
				unsubscribe = nullptr;
			}
		}
	};
	//using SubscriptionToken = _SubscriptionToken;

	/**
	 * @brief Thread-safe event bus.
	 *
	 * QEventBus allows subscribing handlers to event types, posting events to a
	 * queue, and dispatching queued events to all registered handlers. It
	 * supports both synchronous processing via process() and asynchronous
	 * processing via runAsync()/stop().
	 *
	 * Handlers are stored per event type (std::type_index of the concrete
	 * event) and receive events by const-reference to the concrete event type.
	 *
	 * @note All public subscribe/post/process operations are designed to be
	 * thread-safe, but handlers themselves are invoked on the processing
	 * thread/context and must be safe to execute there.
	 */
	class QEventBus {
	public:
		QEventBus(const std::string_view& eventNamespace);

		/**
		 * @brief Alias for an event handler function.
		 * @tparam EventType Concrete event type handled by the function.
		 *
		 * The handler receives a const reference to the event instance.
		 */
		template<typename EventType>
		using Handler = eastl::function<void(const EventType&)>;

		/**
		 * @brief Subscribe a handler to EventType and receive a RAII token.
		 * @tparam EventType The event type to subscribe to.
		 * @param handler The handler called when an EventType is dispatched.
		 * @return SubscriptionToken RAII object that will unsubscribe on destruction.
		 *
		 * The returned token ensures the handler is removed when the token is
		 * destroyed. The unsubscribe action captured in the token is safe to
		 * call from any thread.
		 */
		template<typename EventType>
		SubscriptionToken subscribeRAII(Handler<EventType> handler);

		/**
		 * @brief Subscribe a handler to EventType without RAII token.
		 * @tparam EventType The event type to subscribe to.
		 * @param handler The handler called when an EventType is dispatched.
		 *
		 * Use this if you do not need automated unsubscription. To remove the
		 * handler manually you must extend this class or implement additional
		 * removal logic.
		 */
		template<typename EventType>
		void subscribe(Handler<EventType> handler);

		/**
		 * @brief Post an event instance to the bus for later processing.
		 * @tparam EventType The concrete event type being posted.
		 * @param event The event instance to enqueue (moved or copied as needed).
		 *
		 * The event is stored as a heap-allocated IEvent-derived instance and
		 * will be dispatched during process() or by the asynchronous thread if
		 * runAsync() is active.
		 */
		template<typename EventType>
		void post(EventType&& event);

		/**
		 * @brief Process and dispatch all currently queued events.
		 *
		 * This method will dequeue all pending events and invoke their
		 * registered handlers synchronously in the caller's thread.
		 */
		void process();

		/**
		 * @brief Start asynchronous processing of queued events.
		 *
		 * This creates a background thread that waits for posted events and
		 * calls dispatch on them as they arrive.
		 */
		void runAsync();

		/**
		 * @brief Stop asynchronous processing and join the worker thread.
		 *
		 * Blocks until the background processing thread has exited. After
		 * stop() returns, it is safe to destroy the QEventBus or call
		 * runAsync() again.
		 */
		void stop();

		size_t getQueueSize() const;
		/**
		 * @brief Destructor.
		 *
		 * Ensures asynchronous processing is stopped and resources are cleaned
		 * up. Any remaining queued events will be destroyed.
		 */
		~QEventBus();

		const std::string_view& getNamespace() const;
	private:
		/**
		 * @brief Dispatch a concrete IEvent to all registered handlers for its type.
		 * @param e The event to dispatch.
		 *
		 * This method performs no locking of the event queue; it only queries
		 * the handlers map and invokes handlers. Callers are responsible for
		 * appropriate locking when necessary.
		 */
		void dispatch(const IEvent& e);

		
		struct TypeIndexHash {
			size_t operator()(const std::type_index& t) const noexcept {
				return t.hash_code();
			}
		};

		struct TypeIndexEqual {
			bool operator()(const std::type_index& a, const std::type_index& b) const noexcept {
				return a == b;
			}
		};
		/**
		 * @brief Map of event type to list of handlers accepting IEvent references.
		 *
		 * Key: std::type_index corresponding to the concrete EventType.
		 * Value: vector of callables accepting const IEvent&; callers cast to the concrete type.
		 *
		 * @note Protected by QEventBus::mutex for thread-safe access.
		 */
		eastl::unordered_map<
			std::type_index,
			eastl::vector<eastl::function<void(const IEvent&)>>,
			TypeIndexHash,
			TypeIndexEqual
		> handlers;
		/**
		 * @brief Queue of pending events awaiting processing.
		 *
		 * Each entry is a unique_ptr to an IEvent instance holding the concrete event.
		 *
		 * @note Protected by QEventBus::queueMutex for thread-safe access.
		 */
		eastl::queue<eastl::unique_ptr<IEvent>> queue;

		/**
		 * @brief Mutex protecting the handlers map and subscription modification.
		 */
		std::mutex mutex;

		/**
		 * @brief Mutex protecting the event queue.
		 */
		std::mutex queueMutex;

		/**
		 * @brief Condition variable used to wake the background processing thread.
		 */
		std::condition_variable conditionalVariable;

		/**
		 * @brief Flag indicating whether the asynchronous processing thread should run.
		 */
		std::atomic_bool bIsRunning = false;

		/**
		 * @brief Background thread used for asynchronous processing when runAsync() is called.
		 */
		std::thread thread;

		const std::string_view eventNamespace;
	};
	template<typename EventType>
	inline SubscriptionToken QEventBus::subscribeRAII(Handler<EventType> handler) {
		std::lock_guard<std::mutex> lock(QEventBus::mutex);

		auto& handlers = QEventBus::handlers[typeid(EventType)];

		auto typeIdx = std::type_index(typeid(EventType));

		size_t index = handlers.size();

		handlers.emplace_back([handler](const IEvent& e) {
			handler(static_cast<const EventType&>(e));
			});

		auto it = eastl::prev(handlers.end());

		return SubscriptionToken {
			[this, typeIdx, index]() mutable {
				std::lock_guard<std::mutex> lock(mutex);   // Important: re-lock

				auto mapIt = this->handlers.find(typeIdx);
				if (mapIt == this->handlers.end()) return;

				auto& vec = mapIt->second;
				if (index < vec.size()) {
					vec.erase(vec.begin() + index);

					// Optional: shrink if too many removals, but not necessary
					if (vec.empty()) {
						this->handlers.erase(mapIt);
					}
				}
			}
		};
	}

	template<typename EventType>
	inline void QEventBus::subscribe(Handler<EventType> handler) {
		std::lock_guard<std::mutex> lock(QEventBus::mutex);

		// Simply store the handler � no token or RAII logic.
		auto& handlers = QEventBus::handlers[typeid(EventType)];
		handlers.emplace_back([handler](const IEvent& e) {
			handler(static_cast<const EventType&>(e));
			});
	}

	template<typename EventType>
	inline void QEventBus::post(EventType&& event) {
		// Create an owned copy / move of the event on the heap
		using Decayed = std::decay_t<EventType>;
		auto ptr = eastl::make_unique<Decayed>(eastl::forward<EventType>(event));
		{
			std::lock_guard<std::mutex> lock(QEventBus::queueMutex);
			QEventBus::queue.push(eastl::move(ptr));
		}
		QEventBus::conditionalVariable.notify_one();
	}
}
