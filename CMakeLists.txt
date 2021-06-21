cmake_minimum_required(VERSION 3.17)
# CMake version is set at 3.17 because of find_package(CUDAToolkit)
# TODO: find a workaround to be able to lower the version requirement
project(hwmalloc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE) # AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    set_property(CACHE CMAKE_CXX_FLAGS PROPERTY VALUE "-Wall -Wextra -Wpedantic")
endif()

add_library(hwmalloc SHARED)
#add_library(HWMALLOC::hwmalloc ALIAS hwmalloc)
set_target_properties(hwmalloc PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)

set(HWMALLOC_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${HWMALLOC_MODULE_PATH}")

find_package(NUMA REQUIRED)
target_link_libraries(hwmalloc PRIVATE NUMA::libnuma)
set(HWMALLOC_NUMA_THROWS OFF CACHE BOOL "True if numa_tools may throw during initialization")
if (HWMALLOC_NUMA_THROWS)
    target_compile_definitions(hwmalloc PRIVATE HWMALLOC_NUMA_THROWS)
endif()
find_package(Boost REQUIRED)
target_link_libraries(hwmalloc PRIVATE Boost::boost)

target_include_directories(hwmalloc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_BINARY_DIR}/include)

set(HWMALLOC_MPI_SUPPORT OFF CACHE BOOL "True if MPI support is built")

set(TESTING OFF CACHE BOOL "True if tests shall be built")

set(HWMALLOC_ENABLE_DEVICE OFF CACHE BOOL "True if GPU support shall be enabled")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/hwmalloc/config.hpp @ONLY)

add_subdirectory(src)

if (TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

