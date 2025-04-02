include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

#if(DEFINED ENV{GITHUB_ACTION}) # TODO remove this when CI auth is fixed
#    set(GIT_REPO https://vacmg:$ENV{GH_PAT}@github.com/vacmg/OSInterface.git)
#else()
#    set(GIT_REPO git@github.com:vacmg/OSInterface.git)
#endif()

set(GIT_REPO git@github.com:vacmg/OSInterface.git)

FetchContent_Declare(
        OSInterface
        GIT_REPOSITORY  ${GIT_REPO}
        GIT_TAG         main
)
FetchContent_MakeAvailable(OSInterface)

add_library(LinuxOSInterface STATIC)

file(GLOB LinuxOSInterface_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(LinuxOSInterface PRIVATE ${LinuxOSInterface_SOURCES})
target_include_directories(LinuxOSInterface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(LinuxOSInterface PUBLIC OSInterface)
