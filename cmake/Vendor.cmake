cmake_minimum_required(VERSION 4.0)
include(FetchContent)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/Vendor)

# Rewrite all github ssh URLs to https
execute_process(
        COMMAND git config --global url.https://github.com/.insteadOf git@github.com:
)

FetchContent_Declare(EASTL GIT_REPOSITORY https://github.com/electronicarts/EASTL.git)
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # EACore uses an old version
FetchContent_MakeAvailable(EASTL SFML)