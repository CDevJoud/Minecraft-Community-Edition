#include "ThreadImpl.hpp"
#include "Thread.hpp"

namespace mce::core {
	Thread::~Thread() {
		Thread::wait();
		delete Thread::entryPoint;
		Thread::entryPoint = nullptr;
	}
	void Thread::launch() {
		Thread::wait();
		Thread::pImpl = new ThreadImpl(this);
		if (Thread::pImpl->isRunning()) {

		}
	}
	void Thread::wait() {
		if (Thread::pImpl) {
			Thread::pImpl->terminate();
			delete Thread::pImpl;
			Thread::pImpl = nullptr;
		}
	}
	void Thread::run() {
		if (entryPoint) {
			entryPoint->run();

			// a QEventBus post an event of thread finished
			qBus.post<event::ThreadFinished>(event::ThreadFinished{ this });
		}
	}
	void Thread::terminate() {
		this->pImpl->terminate();
	}
}
