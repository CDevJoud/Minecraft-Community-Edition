cmake_minimum_required(VERSION 4.0)
project("Minecraft-Community-Edition")

set(CMAKE_CXX_STANDARD 26)
set(MCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Minecraft-Community-Edition")
add_subdirectory("${MCE_DIR}/Vendor")

if (WIN32 AND ("${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    set (MCE_EXEC_TYPE WIN32)
endif ()

file(GLOB_RECURSE MCE_SOURCES "${MCE_DIR}/*.cpp" "${MCE_DIR}/*.hpp" "${MCE_DIR}/*.c" "${MCE_DIR}/*.h")
list(FILTER MCE_SOURCES EXCLUDE REGEX "Vendor")
add_executable(${PROJECT_NAME} ${MCE_EXEC_TYPE} ${MCE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${MCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE EASTL SFML::Graphics stdc++exp)