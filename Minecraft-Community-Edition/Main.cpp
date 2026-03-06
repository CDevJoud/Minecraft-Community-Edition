#pragma warning(disable:4996)
#include "Common/Platform.hpp"
#include "Minecraft.hpp"
#include "IO/Logger.hpp"

#include <filesystem>
#include <SFML/System/Sleep.hpp>

#include "IO/LoggerSinks.hpp"

static mce::QEventBus qBus;

static std::string getLogFileName() {
	const auto now = std::chrono::system_clock::now();

	const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	const std::tm* pTime = std::localtime(&currentTime);

	// strftime adds the null terminator, so initializing isn't strictly necessary
	char filename[32];
	std::strftime(filename, sizeof(filename), "logs/%Y-%m-%d %H-%M-%S.txt", pTime);

	return filename;
}

static void setupLogging() {
	namespace fs = std::filesystem;

	if (!fs::is_directory("logs"))
		fs::create_directory("logs");

	eastl::shared_ptr<mce::FileSink> fileSink = eastl::make_shared<mce::FileSink>(getLogFileName());

	mce::Logger& logger = mce::Logger::getGlobalLogger(qBus);
	logger.addSink(fileSink);
}

#if defined(MCE_PLATFORM_WINDOWS) && defined(NDEBUG)
#include <minwindef.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	setupLogging();
	
	mce::Minecraft minecraft;
	minecraft.run();
	return 0;
}
