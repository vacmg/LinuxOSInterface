if (NOT ESP_PLATFORM) # Only configure tests if we are building in a computer.

    project(LinuxOSShim_GoogleTests)
    if (NOT TARGET gtest)
        add_subdirectory(googletest)
    endif ()

    include_directories("${gtest_SOURCE_DIR}/include" ".")

    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

    # adding the Google_Tests_run target
    add_executable(LinuxOSShim_GoogleTestsExe ${TEST_SOURCES} ${TEST_UTILS_SOURCES})
    target_include_directories(LinuxOSShim_GoogleTestsExe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

    # linking Google_Tests_run with LinuxOSShim which will be tested
    target_link_libraries(LinuxOSShim_GoogleTestsExe LinuxOSShim)

    target_link_libraries(LinuxOSShim_GoogleTestsExe gtest gtest_main)
endif ()
