#pragma once
#include <coroutine>
#include <exception>

namespace mce::tasks {
	struct RenderTask {
		struct promise_type {
			RenderTask get_return_object() {
				return RenderTask {std::coroutine_handle<promise_type>::from_promise(*this)};
			}
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			void return_void() {}
			void unhandled_exception() { std::terminate(); }
		};
		
		std::coroutine_handle<promise_type> handle;

		explicit RenderTask(auto h) : handle(h) {}
		~RenderTask() { if (handle) handle.destroy(); }

		void resume() {
			if (handle && !handle.done()) handle.resume();
		}
	};

	struct BgfxShutdown {
		struct promise_type {
			BgfxShutdown get_return_object() {
				return BgfxShutdown{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }

			void return_void() {}
			void unhandled_exception() { std::terminate(); }
		};

		std::coroutine_handle<promise_type> handle;

		explicit BgfxShutdown(auto h) : handle(h) {}
		~BgfxShutdown() { if (handle) handle.destroy(); }

		void resume() {
			if (handle && !handle.done()) handle.resume();
		}
	};
}
