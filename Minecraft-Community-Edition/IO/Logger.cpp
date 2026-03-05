#define _CRT_SECURE_NO_WARNINGS

#include "Logger.hpp"
#include "LoggerSinks.hpp"



#include <ctime>
#include <chrono>
#include <iostream>
#include <print>

namespace MCE {
	namespace Color {
		static const std::string BLUE = "\x1b[38;2;0;120;220m";
		static const std::string GOLD = "\x1b[38;2;255;213;0m";
		static const std::string GREEN = "\x1b[38;2;0;150;60m";
		static const std::string RED = "\x1b[38;2;255;0;0m";
		static const std::string LIGHT_GRAY = "\x1b[38;2;118;118;118m";
		static const std::string DARK_GRAY = "\x1b[38;2;148;148;148m";
		static const std::string RESET = "\x1b[0m";
	}

	Logger::Logger(const std::string_view name, const bool createStdoutSink)
		:
		name(name) {
		if (createStdoutSink)
			sinks.emplace_back(eastl::make_shared<StdoutSink>());
	}

	void Logger::addSink(const eastl::shared_ptr<LoggerSink>& sink) {
		sinks.emplace_back(sink);
	}

	void Logger::log(const LogLevel level, const std::string_view message) {
		std::string severity;
		std::string color;
		switch (level) {
		case LogLevel::INFO: {
			severity = "INFO";
			color = Color::BLUE;
		}
		break;

		case LogLevel::WARN: {
			severity = "WARN";
			color = Color::GOLD;
		}
		break;

		case LogLevel::DEBUG: {
			severity = "DEBUG";
			color = Color::GREEN;
		}
		break;

		case LogLevel::ERROR: {
			severity = "ERROR";
			color = Color::RED;
		}
		break;
		}

		const std::string colorized = std::format("{0}[{1}{2}{0}] [{1}{3}{0}] {4}: {5}[{6}] => {7} {4}",
			Color::LIGHT_GRAY, Color::DARK_GRAY, GetFormattedTime(), name, Color::RESET, color, severity, message);
		const std::string plain = std::format("[{0}] [{1}] : [{2}] => {3}",
			GetFormattedTime(), name, severity, message);

		for (const auto& sink : sinks)
			sink->log(colorized, plain);
	}

	std::string Logger::GetFormattedTime() {
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* pTime = std::localtime(&currentTime);

		// strftime adds the null terminator, so initializing isn't strictly necessary
		char time[32];
		std::strftime(time, sizeof(time), "%H:%M:%S", pTime);

		return time;
	}

	Logger& Logger::getGlobalLogger() {
		static Logger globalLogger("MCE");
		return globalLogger;
	}
}
