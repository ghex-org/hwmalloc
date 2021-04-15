cmake_minimum_required(VERSION 3.14.5)
project(hwmalloc LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(hwmalloc SHARED)
#add_library(HWMALLOC::hwmalloc ALIAS hwmalloc)

target_include_directories(hwmalloc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

set(TESTING OFF CACHE BOOL "True if tests shall be built")

add_subdirectory(src)

if (TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
