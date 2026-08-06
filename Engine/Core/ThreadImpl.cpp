#include "..\Platform.hpp"

#ifdef MCE_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
#include <assert.h>
#include "Thread.hpp"
#include "ThreadImpl.hpp"

namespace dms::core {
	ThreadImpl::ThreadImpl(Thread* owner) {
		ThreadImpl::hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &ThreadImpl::entryPoint, owner, 0, &threadID));

		if (!ThreadImpl::hThread) {
			// error could not create a thread
		}
	}

	ThreadImpl::~ThreadImpl() {
		if (ThreadImpl::hThread) {
			CloseHandle(ThreadImpl::hThread);
			ThreadImpl::hThread = nullptr;
		}
	}

	void ThreadImpl::wait() {
		if (ThreadImpl::hThread) {
			assert(ThreadImpl::hThread != GetCurrentThread());
			WaitForSingleObject(ThreadImpl::hThread, INFINITE);
		}
	}

	void ThreadImpl::terminate() {
		if (ThreadImpl::hThread) {
			TerminateThread(ThreadImpl::hThread, 0);
		}
	}

	bool ThreadImpl::isRunning() const {
		return (ThreadImpl::hThread);
	}

	inline unsigned int __stdcall ThreadImpl::entryPoint(void* userData) {

		Thread* owner = static_cast<Thread*>(userData);

		owner->run();

		ExitThread(0);
		return 0;
	}
}

#elif defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)

#include "ThreadImpl.hpp"
#include <assert.h>
#include "Thread.hpp"

namespace dms {
	ThreadImpl::ThreadImpl(Thread* owner) {
		ThreadImpl::isActive = pthread_create(&thread, NULL, &ThreadImpl::entryPoint, owner) == 0;

		if (!ThreadImpl::isActive) {
			// error could not create a thread
		}
	}

	void ThreadImpl::wait() {
		if (ThreadImpl::isActive) {
			assert(pthread_equal(pthread_self(), thread) == 0); // A thread cannot wait for itself!
			pthread_join(thread, NULL);
		}
	}

	void ThreadImpl::terminate() {
		if (ThreadImpl::isActive) {
			// See https://stackoverflow.com/questions/4610086/pthread-cancel-al
			pthread_kill(m_thread, SIGUSR1);
		}
	}

	bool ThreadImpl::isRunning() const {
		return (ThreadImpl::isActive);
	}

	inline unsigned int __stdcall ThreadImpl::entryPoint(void* userData) {

		Thread* owner = static_cast<Thread*>(userData);

		owner->run();

		ExitThread(0);
		return 0;
	}
}

#endif
