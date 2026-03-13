#pragma once
#include "..\Common\Platform.hpp"
#if defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
#include <pthread.h>
#endif

/**
 * @file ThreadImpl.hpp
 * @brief Platform-specific implementation details for mce::Thread.
 *
 * Contains the PIMPL-like backing class used by mce::Thread to manage
 * platform thread handles and lifecycle operations.
 */

namespace mce {
	/**
	 * @brief Forward declaration of the high-level Thread wrapper.
	 *
	 * The real Thread class owns a ThreadImpl instance which performs
	 * platform-specific thread creation, waiting and termination.
	 */
	class Thread;

	/**
	 * @brief Platform-specific thread implementation.
	 *
	 * This class encapsulates native thread handles and provides a small
	 * interface for the Thread wrapper to control thread lifetime.
	 *
	 * It is intentionally non-copyable (implicitly, due to native handles).
	 */
	class ThreadImpl {
	public:
		/**
		 * @brief Construct a ThreadImpl bound to its owner.
		 * @param owner Pointer to the high-level Thread object that owns this implementation.
		 *
		 * The owner is stored by reference only for use inside the static
		 * entryPoint function to call back into Thread::run() or equivalent.
		 */
		ThreadImpl(Thread* owner);

#ifdef MCE_PLATFORM_WINDOWS
		/**
		 * @brief Destructor for Windows platform.
		 *
		 * Ensures native thread handle resources are closed if necessary.
		 * On Windows this may close the thread handle; the thread may already
		 * have exited. Thread lifetime semantics are coordinated with mce::Thread.
		 */
		~ThreadImpl();
#endif

		/**
		 * @brief Block until the associated thread exits.
		 *
		 * On success this method returns after the remote thread has terminated.
		 * Behavior and error handling are platform-specific.
		 */
		void wait();

		/**
		 * @brief Request termination of the associated thread.
		 *
		 * Implementations may signal the thread to stop or forcibly terminate
		 * the native thread handle depending on platform support and usage.
		 *
		 * @note Prefer cooperative shutdown via shared flags when possible.
		 */
		void terminate();


		bool isRunning() const;
	private:

		/**
		 * @brief Native thread entry point adapter.
		 * @param userData Pointer to user-provided data (usually a Thread*).
		 * @return Platform-specific thread return value.
		 *
		 * This static function adapts the native thread signature to call into
		 * the owning Thread object's run/entry routine.
		 */
		static unsigned int __stdcall entryPoint(void* userData);

#ifdef MCE_PLATFORM_WINDOWS
		/**
		 * @brief Native handle to the created thread (Windows HANDLE represented as void*).
		 *
		 * This handle may be used with native wait functions and must be closed
		 * when no longer needed to avoid handle leaks.
		 */
		void* hThread;

		/**
		 * @brief Native thread identifier (Windows DWORD stored as unsigned int).
		 *
		 * Useful for debugging and certain platform APIs that accept a thread id.
		 */
		unsigned int threadID;
#elif defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
		/**
		 * @brief Native pthread handle for POSIX platforms.
		 *
		 * Used with pthread_join, pthread_cancel, etc. depending on lifecycle.
		 */
		pthread_t thread;

		/**
		 * @brief Flag indicating whether the pthread is active.
		 *
		 * This is an implementation detail used to avoid joining or destroying
		 * a thread that has already finished. Access to this flag should be
		 * properly synchronized by the owner when required.
		 */
		bool isActive;
#endif
	};
}
