cmake_minimum_required(VERSION 3.16)

Project(LinuxOSShim)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/OSShim)

add_library(LinuxOSShim STATIC)

file(GLOB LinuxOSShim_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(LinuxOSShim PRIVATE ${LinuxOSShim_SOURCES})
target_include_directories(LinuxOSShim PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(LinuxOSShim PUBLIC OSShim)

add_subdirectory(Tests)
