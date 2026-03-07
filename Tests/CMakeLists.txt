cmake_minimum_required(VERSION 4.0)
project("Tests")
enable_testing()
include(GoogleTest)

function(DeclareTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} GTest::gtest_main Minecraft-Community-Edition-lib)
    gtest_discover_tests(${name})
endfunction()

DeclareTest(InputStream)
DeclareTest(Logging)
