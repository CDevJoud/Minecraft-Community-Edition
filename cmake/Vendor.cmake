cmake_minimum_required(VERSION 4.0)
include(FetchContent)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/Vendor)

# Rewrite all github ssh URLs to https
execute_process(
        COMMAND git config --global url.https://github.com/.insteadOf git@github.com:
)

cmake_policy(SET CMP0135 NEW) # Fix DOWNLOAD_EXTRACT_TIMESTAMP warning
FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
        SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/googletest
        BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/googletest-build
        SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/googletest-subbuild
)
FetchContent_Declare(EASTL GIT_REPOSITORY https://github.com/electronicarts/EASTL.git)
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # EACore uses an old version
FetchContent_MakeAvailable(EASTL SFML)

if (MCE_BUILD_TESTS)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif ()