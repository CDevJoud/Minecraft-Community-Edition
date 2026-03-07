#include <gtest/gtest.h>
#include "IO/Logger.hpp"

TEST(InputStream, normal) {
	MCE_INFO("Hello, {}", "world!");
	MCE_WARN("Hello, {}", "world!");
	MCE_ERROR("Hello, {}", "world!");
	MCE_DEBUG("Hello, {}", "world!");
}

TEST(InputStream, trace) {
	MCE_INFO_TRACE("Hello, {}", "world!");
	MCE_WARN_TRACE("Hello, {}", "world!");
	MCE_ERROR_TRACE("Hello, {}", "world!");
	MCE_DEBUG_TRACE("Hello, {}", "world!");
}
