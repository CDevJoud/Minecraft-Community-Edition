#pragma once

#include <sstream>
#include <string>

#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"

#ifndef NDEBUG
#define MCE_INFO(...) MCE::Logger::getGlobalLogger().info(__VA_ARGS__)
#define MCE_WARN(...) MCE::Logger::getGlobalLogger().warn(__VA_ARGS__)
#define MCE_ERROR(...) MCE::Logger::getGlobalLogger().error(__VA_ARGS__)
#define MCE_DEBUG(...) MCE::Logger::getGlobalLogger().debug(__VA_ARGS__)
#else
#define MCE_INFO(...)
#define MCE_WARN(...)
#define MCE_ERROR(...)
#define MCE_DEBUG(...)
#endif

namespace MCE {
	class LoggerSink;

	enum class LogLevel {
		INFO,
		WARN,
		ERROR,
		DEBUG,
	};

	class Logger {
	public:
		explicit Logger(std::string_view name, bool createStdoutSink = true);

		void addSink(const eastl::shared_ptr<LoggerSink>& sink);
		void log(LogLevel level, std::string_view message);

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

		static Logger& getGlobalLogger();
	private:
		static std::string GetFormattedTime();

		std::string name;
		eastl::vector<eastl::shared_ptr<LoggerSink>> sinks;
	};
}
