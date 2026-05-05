#pragma warning(disable:4996)
#include "FileLogger.hpp"
#include <chrono>
namespace mce::io {
	FileLogger::FileLogger(core::QEventBus& qBus) {
		qBus.subscribe<event::Log>([this](const auto& e) {
			logCallback(e);
			});
	}

	FileLogger::~FileLogger() {
		fOut.close();
	}

	void FileLogger::open(const std::string& fName) {
		fOut.open(fName);
	}

	std::string getFormattedTime() {
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* pTime = std::localtime(&currentTime);

		// strftime adds the null terminator, so initializing isn't strictly necessary
		char time[32];
		std::strftime(time, sizeof(time), "%H:%M:%S", pTime);

		return time;
	}

	void FileLogger::logCallback(const event::Log& e) {
		if (fOut.isOpen()) {

			auto strTime = "[" + getFormattedTime() + "] ";

			fOut.write(strTime.c_str(), strTime.length());

			std::string channelName = e.channel;

			fOut.write(channelName.c_str(), channelName.length());

			switch (e.severity) {
			default:
				break;

			case event::Log::Severity::DEBUG:
				fOut.write(" [DEBUG] ", 10);
				break;
			case event::Log::Severity::ERROR:
				fOut.write(" [ERROR] ", 10);
				break;
			case event::Log::Severity::INFO:
				fOut.write(" [INFO] ", 9);
				break;
			case event::Log::Severity::WARN:
				fOut.write(" [WARN] ", 9);
				break;
			}
			auto msg = " => " + e.msg;
			fOut.write(msg.c_str(), msg.length());

			fOut.write("\r\n", 1);
		}
	}
}
