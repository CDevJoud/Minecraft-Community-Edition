#pragma once

#include <sstream>
#include <string>
#include <format>
#include <source_location>

#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"

#include "Core/QEventBus.hpp"
#include "Core/IEvent.hpp"

#define MCE_LOGGER_SET_QEVENTBUS 0

#ifndef NDEBUG
#define MCE_INFO(...) mce::Logger::getGlobalLogger(qBus).info(__VA_ARGS__)
#define MCE_WARN(...) mce::Logger::getGlobalLogger(qBus).warn(__VA_ARGS__)
#define MCE_ERROR(...) mce::Logger::getGlobalLogger(qBus).error(__VA_ARGS__)
#define MCE_DEBUG(...) mce::Logger::getGlobalLogger(qBus).debug(__VA_ARGS__)
#define MCE_INFO_TRACE(...) mce::Logger::getGlobalLogger(qBus).info_trace(std::source_location::current(), __VA_ARGS__)
#define MCE_WARN_TRACE(...) mce::Logger::getGlobalLogger(qBus).warn_trace(std::source_location::current(), __VA_ARGS__)
#define MCE_ERROR_TRACE(...) mce::Logger::getGlobalLogger(qBus).error_trace(std::source_location::current(), __VA_ARGS__)
#define MCE_DEBUG_TRACE(...) mce::Logger::getGlobalLogger(qBus).debug_trace(std::source_location::current(), __VA_ARGS__)
#else
#define MCE_INFO(...)
#define MCE_WARN(...)
#define MCE_ERROR(...)
#define MCE_DEBUG(...)
#define MCE_INFO_TRACE(...)
#define MCE_WARN_TRACE(...)
#define MCE_ERROR_TRACE(...)
#define MCE_DEBUG_TRACE(...)
#endif

namespace mce {
	class LoggerSink;

	enum class LogLevel {
		INFO,
		WARN,
		ERROR,
		DEBUG,
	};

	class Logger {
	public:
		explicit Logger(QEventBus& qBus, std::string_view name, bool createStdoutSink = true);

		void addSink(const eastl::shared_ptr<LoggerSink>& sink);
		void log(LogLevel level, std::string_view message, eastl::optional<std::source_location> location = eastl::nullopt);

		template <typename... Args>
		void info(std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::INFO, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void warn(std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::WARN, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void error(std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::ERROR, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void debug(std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::DEBUG, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void info_trace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::INFO, std::format(format, std::forward<Args>(args)...), location);
		}
		
		template <typename... Args>
		void warn_trace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::WARN, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void error_trace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::ERROR, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void debug_trace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			log(LogLevel::DEBUG, std::format(format, std::forward<Args>(args)...), location);
		}

		//static QEventBus& getGlobalEventBus();
		static Logger& getGlobalLogger(QEventBus& qBus);
	private:
		/*class LoggerEventBus : public QEventBus {
		public:
			LoggerEventBus();
		};*/

		void logCallback(const event::LoggerOutput& e);
		static std::string getFormattedSource(const std::source_location& location);

		static std::string GetFormattedTime();
		const size_t MAX_LOG_EVENTS = 10000;
		std::string name;

		eastl::vector<eastl::shared_ptr<LoggerSink>> sinks;
		QEventBus& qBus;

		SubscriptionToken busSubscription;
	};
}
