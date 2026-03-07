cmake_minimum_required(VERSION 4.0)
project("InputStream")
include(GoogleTest)

add_executable(${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} GTest::gtest_main)