#include <gtest/gtest.h>

#include "IO/Stream/FileInputStream.hpp"

using namespace mce;

TEST(InputStream, Reading) {
	FileInputStream testStream("TestFiles/test.txt");

	EXPECT_TRUE(testStream.isOpen());
	EXPECT_FALSE(testStream.eof());

	std::vector<char> fileContent(testStream.getSize() + 1, 0);
	size_t bytesRead = testStream.read(fileContent.data(), testStream.getSize());

	std::ifstream stream("TestFiles/test.txt");

	std::string content;
	while (std::getline(stream, content)) {}

	EXPECT_EQ(content, fileContent.data());
	EXPECT_EQ(bytesRead, content.size());

	stream.close();
}

TEST(InputStream, ReadMoreThanFileHas) {
	FileInputStream testStream("TestFiles/test.txt");

	std::vector<char> fileContent(testStream.getSize() + 1, 0);
	size_t bytesRead = testStream.read(fileContent.data(), testStream.getSize() + 2);

	EXPECT_EQ(bytesRead, testStream.getSize());
}