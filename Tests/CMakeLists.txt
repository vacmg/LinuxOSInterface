if (NOT ESP_PLATFORM) # Only configure tests if we are building in a computer.

    project(LinuxOSInterface_GoogleTests)

    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY  https://github.com/google/googletest.git
            GIT_TAG         v1.17.0
    )
    FetchContent_MakeAvailable(googletest)

    include_directories("${gtest_SOURCE_DIR}/include" ".")

    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

    # adding the Google_Tests_run target
    add_executable(LinuxOSInterface_GoogleTestsExe ${TEST_SOURCES} ${TEST_UTILS_SOURCES})
    target_include_directories(LinuxOSInterface_GoogleTestsExe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

    # linking Google_Tests_run with LinuxOSInterface which will be tested
    target_link_libraries(LinuxOSInterface_GoogleTestsExe LinuxOSInterface)

    target_link_libraries(LinuxOSInterface_GoogleTestsExe gtest gtest_main)
endif ()
