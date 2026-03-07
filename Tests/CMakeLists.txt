cmake_minimum_required(VERSION 4.0)
project("Tests")
enable_testing()
include(GoogleTest)

function(DeclareTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} GTest::gtest Minecraft-Community-Edition-lib)
    gtest_discover_tests(${name})
endfunction()

function(CopyFilesFor target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/TestFiles
            $<TARGET_FILE_DIR:${target}>/TestFiles
    )
endfunction()

DeclareTest(FileStreams)
DeclareTest(Logging)

CopyFilesFor(FileStreams)

file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(AllTests ${TEST_FILES})

target_link_libraries(AllTests Minecraft-Community-Edition-lib gtest gtest_main)
