#define _CRT_SECURE_NO_WARNINGS

#include "Logger.hpp"
#include "LoggerSinks.hpp"

#include <ctime>
#include <chrono>
#include <iostream>
#include <print>

namespace mce {
	namespace Color {
		static const std::string BLUE = "\x1b[38;2;0;120;220m";
		static const std::string GOLD = "\x1b[38;2;255;213;0m";
		static const std::string GREEN = "\x1b[38;2;0;150;60m";
		static const std::string RED = "\x1b[38;2;255;0;0m";
		static const std::string LIGHT_GRAY = "\x1b[38;2;118;118;118m";
		static const std::string DARK_GRAY = "\x1b[38;2;148;148;148m";
		static const std::string RESET = "\x1b[0m";
	}

	Logger::Logger(QEventBus& qBus, const std::string_view name, const bool createStdoutSink)
		:
		name(name), qBus(qBus) {
		if (createStdoutSink)
			sinks.emplace_back(eastl::make_shared<StdoutSink>());

		qBus.subscribe<event::LoggerOutput>([this](const event::LoggerOutput& e) {
			logCallback(e);
		});
	}

	void Logger::addSink(const eastl::shared_ptr<LoggerSink>& sink) {
		sinks.emplace_back(sink);
	}

	void Logger::log(const LogLevel level, std::string_view message, const std::source_location& location) {
		//Check the event queue size and make sure to not accedintly flood it
		if (Logger::qBus.getQueueSize() >= Logger::MAX_LOG_EVENTS)
			return; //Ignore the log

		event::LoggerOutput lOut;
		lOut.severity = static_cast<event::LoggerOutput::Severity>(level);
		lOut.msg = message;
		lOut.location = location;
		qBus.post(lOut);
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

	QEventBus & Logger::getGlobalEventBus() {
		static QEventBus loggerEventBus;
		return loggerEventBus;
	}

	Logger& Logger::getGlobalLogger() {
		static Logger globalLogger(getGlobalEventBus(), "MCE");
		return globalLogger;
	}

	void Logger::logCallback(const event::LoggerOutput &e) {
		std::string severity;
		std::string color;
		using Severity = event::LoggerOutput::Severity;
		switch (static_cast<event::LoggerOutput::Severity>(e.severity)) {
			case Severity::INFO:
			{
				severity = "INFO";
				color = Color::BLUE;
			}
				break;

			case Severity::WARN:
			{
				severity = "WARN";
				color = Color::GOLD;
			}
				break;

			case Severity::DEBUG:
			{
				severity = "DEBUG";
				color = Color::GREEN;
			}
				break;

			case Severity::ERROR:
			{
				severity = "ERROR";
				color = Color::RED;
			}
				break;
			case Severity::FATAL: {
				severity = "FATAL";
				color = Color::RED;
			}
				break;
		}

		const std::string file = e.location.file_name();
		const std::string function = e.location.function_name();
		const uint32_t line = e.location.line();

		const std::string colorized = std::format(
			"{0}[{1}{2}{0}] [{1}{3}{0}] {4}: {5}[{6}] {7}:{8} ({9}) => {10} {4}",
			Color::LIGHT_GRAY,
			Color::DARK_GRAY,
			GetFormattedTime(),
			this->name,
			Color::RESET,
			color,
			severity,
			file,
			line,
			function,
			e.msg
		);

		const std::string plain = std::format(
			"[{0}] [{1}] [{2}] {3}:{4} ({5}) => {6}",
			GetFormattedTime(),
			this->name,
			severity,
			file,
			line,
			function,
			e.msg
		);

		for (const auto& sink : sinks) {
			sink->log(colorized, plain);
		}
	}
}
