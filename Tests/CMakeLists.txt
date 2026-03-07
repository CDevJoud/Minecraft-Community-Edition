cmake_minimum_required(VERSION 4.0)
project("Tests")
enable_testing()
include(GoogleTest)

function(DeclareTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} GTest::gtest_main Minecraft-Community-Edition-lib)
    gtest_discover_tests(${name})
endfunction()

function(CopyFilesFor target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/TestFiles
            $<TARGET_FILE_DIR:${target}>/TestFiles
    )
endfunction()

DeclareTest(InputStream)
DeclareTest(Logging)

CopyFilesFor(InputStream)

