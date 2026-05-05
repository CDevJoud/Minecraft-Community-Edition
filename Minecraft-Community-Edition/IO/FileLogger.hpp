#pragma once
#include "Stream/FileOutputStream.hpp"
#include "Core/QEventBus.hpp"

namespace mce::io {
	class FileLogger {
	public:
		FileLogger(core::QEventBus& qBus);
		~FileLogger();

		void open(const std::string& fName);
	private:
		void logCallback(const event::Log& e);
		stream::FileOutputStream fOut;
	};
}
