#include <gtest/gtest.h>

#include "IO/Stream/FileInputStream.hpp"

using namespace mce;

TEST(FileStreams, Reading) {
	FileInputStream testStream("TestFiles/test.txt");

	EXPECT_TRUE(testStream.isOpen());
	EXPECT_FALSE(testStream.eof());

	std::vector<char> fileContent(testStream.getSize() + 1, 0);
	size_t bytesRead = testStream.read(fileContent.data(), testStream.getSize());

	EXPECT_STREQ("this is some data", fileContent.data());
	EXPECT_EQ(bytesRead, 17);
}

TEST(FileStreams, ReadMoreThanFileHas) {
	FileInputStream testStream("TestFiles/test.txt");

	std::vector<char> fileContent(testStream.getSize() + 1, 0);
	const size_t bytesRead = testStream.read(fileContent.data(), testStream.getSize() + 2);

	EXPECT_EQ(bytesRead, testStream.getSize());
}

