#include <gtest/gtest.h>
#include "IO/Logger.hpp"

TEST(Logging, normal) {
	MCE_INFO("Hello, {}", "world!");
	MCE_WARN("Hello, {}", "world!");
	MCE_ERROR("Hello, {}", "world!");
	MCE_DEBUG("Hello, {}", "world!");
}

TEST(Logging, trace) {
	MCE_INFO_TRACE("Hello, {}", "world!");
	MCE_WARN_TRACE("Hello, {}", "world!");
	MCE_ERROR_TRACE("Hello, {}", "world!");
	MCE_DEBUG_TRACE("Hello, {}", "world!");
}
