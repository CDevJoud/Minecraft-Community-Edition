cmake_minimum_required(VERSION 4.0)
include(FetchContent)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareGitRepo libName gitRepo)
    FetchContent_Declare(
            ${libName}
            GIT_REPOSITORY ${gitRepo}
            SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/${libName}
            BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-build
            SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-subbuild
    )
endfunction()

DeclareGitRepo(EASTL https://github.com/electronicarts/EASTL.git)
DeclareGitRepo(SFML https://github.com/SFML/SFML.git)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # EACore uses an old version
FetchContent_MakeAvailable(EASTL)
FetchContent_MakeAvailable(SFML)