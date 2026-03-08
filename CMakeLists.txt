cmake_minimum_required(VERSION 4.0)
project("Minecraft-Community-Edition")

option(MCE_BUILD_TESTS "Set this to ON to build tests" OFF)
set(CMAKE_CXX_STANDARD 26)
set(MCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Minecraft-Community-Edition")

include(cmake/Vendor.cmake)

if (WIN32 AND ("${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    set (MCE_EXEC_TYPE WIN32)
endif ()

file(GLOB_RECURSE MCE_SOURCES "${MCE_DIR}/*.cpp" "${MCE_DIR}/*.hpp" "${MCE_DIR}/*.c" "${MCE_DIR}/*.h")
list(FILTER MCE_SOURCES EXCLUDE REGEX "Main.cpp")
add_library(${PROJECT_NAME}-lib STATIC ${MCE_SOURCES})
add_executable(${PROJECT_NAME} ${MCE_EXEC_TYPE} "${MCE_DIR}/Main.cpp")

check_cxx_compiler_flag(-Wdeprecated-literal-operator HAS_DEP_LITERAL_OP)
if (HAS_DEP_LITERAL_OP)
    target_compile_options(${PROJECT_NAME}-lib PRIVATE -Wno-deprecated-literal-operator)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-literal-operator)
endif ()

target_include_directories(${PROJECT_NAME}-lib PUBLIC ${MCE_DIR})
target_link_libraries(${PROJECT_NAME}-lib PUBLIC EASTL SFML::Graphics stdc++exp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-lib)

if (MCE_BUILD_TESTS)
    add_subdirectory(Tests)
endif ()