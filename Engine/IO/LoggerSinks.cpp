#include "LoggerSinks.hpp"

#include <iostream>
#include <print>

namespace dms::io {
	//void StdoutSink::log(std::string_view prettyMessage, std::string_view) {
	//	//std::println("{}", prettyMessage);

	//	// Flush the buffer manually. Necessary on non-TTY terminals.
	//	// This could be converted later to a dedicated flusher which
	//	// periodically flushes the buffer if the program has logged something
	//	// if logging speed becomes a concern.
	//	std::cout.flush();
	//}

	FileSink::FileSink(const std::string_view filename) {
		file.open(filename.data());
	}

	void FileSink::log(std::string_view, const std::string_view basicMessage) {
		file << basicMessage << std::endl;
	}
}
