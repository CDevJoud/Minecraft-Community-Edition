#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include <EASTL/string_view.h>

namespace MCE {
	enum class LogLevel;

	class LoggerSink {
	public:
		virtual ~LoggerSink() = default;
		virtual void log(std::string_view prettyMessage, std::string_view basicMessage) = 0;
	};

	class StdoutSink final : public LoggerSink {
	public:
		void log(std::string_view prettyMessage, std::string_view basicMessage) override;
	};

	class FileSink final : public LoggerSink {
	public:
		explicit FileSink(std::string_view filename);

		void log(std::string_view prettyMessage, std::string_view basicMessage) override;
	private:
		std::string filename;
		std::ofstream file;
	};
}
