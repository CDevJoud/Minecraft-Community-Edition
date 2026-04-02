#pragma once
#include "QEventBus.hpp"

/**
 * @file Thread.hpp
 * @brief High-level Thread wrapper and callable adapters used by the project.
 *
 * Contains type-erased callable adapters (ThreadFunc and derivatives) and the
 * Thread class which holds a platform-specific implementation pointer
 * (ThreadImpl) and a QEventBus reference for posting lifecycle events.
 */

namespace mce::core {
	class ThreadImpl;

	/**
	 * @brief Abstract type-erased callable interface used as a thread entry point.
	 *
	 * Concrete adapters derive from ThreadFunc and implement run() to invoke the
	 * wrapped callable.
	 */
	struct ThreadFunc {
		/**
		 * @brief Virtual destructor to allow safe deletion via base pointer.
		 */
		virtual ~ThreadFunc() {}

		/**
		 * @brief Execute the wrapped callable.
		 *
		 * Implementations must run the stored callable when this method is invoked.
		 */
		virtual void run() = 0;
	};

	/**
	 * @brief Adapter that wraps a generic functor/lambda.
	 * @tparam F Type of the functor or lambda.
	 */
	template<typename F>
	struct ThreadFunctor : ThreadFunc {
		/**
		 * @brief Construct the adapter with the provided functor.
		 * @param functor The functor or lambda to invoke from run().
		 */
		ThreadFunctor(F functor) : functor(functor) {}

		/**
		 * @brief Invoke the stored functor.
		 */
		virtual void run() override { functor(); };

		/** @brief The stored functor instance. */
		F functor;
	};

	/**
	 * @brief Adapter that wraps a member function without extra arguments.
	 * @tparam C Class type that owns the member function.
	 */
	template<typename C>
	struct ThreadMemberFunctor : ThreadFunc {
		/**
		 * @brief Construct with member function pointer and target object.
		 * @param function Member function pointer to call.
		 * @param object Target object on which to call the member function.
		 */
		ThreadMemberFunctor(void(C::* function)(), C* object) : functor(function), object(object) {}

		/**
		 * @brief Invoke the stored member function on the stored object.
		 *
		 * Note: the exact invocation expression depends on how the pointer-to-member
		 * is stored and used by the adapter.
		 */
		virtual void run() override { functor(); };

		/** @brief Pointer to the member function to invoke. */
		void(C::* functor)();

		/** @brief Pointer to the object instance used when invoking the member function. */
		C* object;
	};

	/**
	 * @brief Adapter that wraps a member function with arbitrary arguments.
	 * @tparam C Class type that owns the member function.
	 * @tparam Args Types of the arguments to forward when invoking the member function.
	 */
	template<typename C, typename... Args>
	struct ThreadMemberFunctorWithArgs : ThreadFunc {
		/**
		 * @brief Construct with member function pointer, target object and arguments.
		 * @param function Member function pointer to call.
		 * @param object Target object on which to call the member function.
		 * @param args Arguments to forward when invoking the member function.
		 */
		ThreadMemberFunctorWithArgs(void(C::* function)(Args...), C* object, Args&&... args) : functor(functor), object(object) {}

		/**
		 * @brief Invoke the stored member function forwarding the stored arguments.
		 */
		virtual void run() override {
			call(std::index_sequence_for<Args...>{});
		}

		/** @brief Pointer to the member function to invoke. */
		void(C::* functor)(Args...);

		/** @brief Pointer to the object instance used when invoking the member function. */
		C* object;

		/** @brief Tuple holding the arguments to forward to the member function. */
		std::tuple<Args...> args;
	private:
		/**
		 * @brief Helper that expands the stored argument tuple and calls the member function.
		 * @tparam I Indices used to unpack the tuple.
		 */
		template <std::size_t... I>
		void call(std::index_sequence<I...>) {
			(object->*functor)(std::get<I>(args)...);
		}
	};

	/**
	 * @brief Lightweight high-level thread wrapper.
	 *
	 * Stores a pointer to a platform-specific implementation (ThreadImpl) and a
	 * type-erased callable adapter (ThreadFunc) representing the entry point.
	 * Use the constructors to create a Thread from lambdas, functors, member
	 * functions, or member functions with arguments.
	 */
	class Thread {
	public:
		/**
		 * @brief Construct a Thread from a generic callable (lambda, functor, function pointer).
		 * @tparam F Type of the callable.
		 * @param qBus Reference to the QEventBus used for thread lifecycle events.
		 * @param function The callable to execute when the thread runs.
		 */
		template<typename F>
		Thread(QEventBus& qBus, F function);

		/**
		 * @brief Construct a Thread that will call a parameterless member function.
		 * @tparam C Class type owning the member function.
		 * @param qBus Reference to the QEventBus used for thread lifecycle events.
		 * @param function Pointer to the member function to invoke.
		 * @param object Pointer to the object instance on which to invoke the member function.
		 */
		template<typename C>
		Thread(QEventBus& qBus, void(C::* function)(), C* object);

		/**
		 * @brief Construct a Thread that will call a member function with arguments.
		 * @tparam C Class type owning the member function.
		 * @tparam Args Types of the arguments to forward when invoking the member function.
		 * @param qBus Reference to the QEventBus used for thread lifecycle events.
		 * @param function Pointer to the member function to invoke.
		 * @param object Pointer to the object instance on which to invoke the member function.
		 * @param args Arguments to forward to the member function when the thread runs.
		 */
		template<typename C, typename... Args>
		Thread(QEventBus& qBus, void(C::* function)(), C* object, Args&&... args);

		/**
		 * @brief Destructor; cleans up stored callable and implementation pointer.
		 */
		~Thread();

		/**
		 * @brief Start execution of the stored callable on a new thread.
		 */
		void launch();

		/**
		 * @brief Block until the thread has finished executing.
		 */
		void wait();

	private:
		friend class ThreadImpl;

		/**
		 * @brief Internal run method called by the platform-specific entry point.
		 *
		 * Invokes the stored ThreadFunc::run() and posts a ThreadFinished event
		 * to the associated QEventBus when execution completes.
		 */
		void run();

		/** @brief Pointer to the platform-specific implementation managing the native thread. */
		ThreadImpl* pImpl;//pointer to implementation

		/** @brief Pointer to the type-erased callable adapter executed by this thread. */
		ThreadFunc* entryPoint;

		/** @brief Reference to the QEventBus used to post thread lifecycle events. */
		QEventBus& qBus;
	};
	template<typename F>
	inline Thread::Thread(QEventBus& qBus, F function) :
		pImpl(nullptr),
		entryPoint(new ThreadFunctor<F>(function)),
		qBus(qBus) {

	}
	template<typename C>
	inline Thread::Thread(QEventBus& qBus, void(C::* function)(), C* object) :
		pImpl(nullptr),
		entryPoint(new ThreadMemberFunctor<C>(function, object)),
		qBus(qBus) {

	}
	template<typename C, typename ...Args>
	inline Thread::Thread(QEventBus& qBus, void(C::* function)(), C* object, Args && ...args) : 
		pImpl(nullptr),
		entryPoint(new ThreadMemberFunctorWithArgs<C>(function, object)),
		qBus(qBus) {

	}
}
