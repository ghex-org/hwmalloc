cmake_minimum_required(VERSION 3.17)
# CMake version is set at 3.17 because of find_package(CUDAToolkit)
# TODO: find a workaround to be able to lower the version requirement
project(hwmalloc VERSION 0.1 LANGUAGES CXX)

# ---------------------------------------------------------------------
# CMake setup, C++ version, build type, modules, etc
# ---------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE) # AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    set_property(CACHE CMAKE_CXX_FLAGS PROPERTY VALUE "-Wall -Wextra -Wpedantic")
endif()

set(HWMALLOC_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${HWMALLOC_MODULE_PATH}")

# ---------------------------------------------------------------------
# Define main hwmalloc library
# ---------------------------------------------------------------------
add_library(hwmalloc SHARED)
#add_library(HWMALLOC::hwmalloc ALIAS hwmalloc)
set_target_properties(hwmalloc PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
include(hwmalloc_macros)

# ---------------------------------------------------------------------
# NUMA setup
# ---------------------------------------------------------------------
find_package(NUMA REQUIRED)
target_link_libraries(hwmalloc PRIVATE NUMA::libnuma)
set(HWMALLOC_NUMA_THROWS OFF CACHE BOOL "True if numa_tools may throw during initialization")
if (HWMALLOC_NUMA_THROWS)
    target_compile_definitions(hwmalloc PRIVATE HWMALLOC_NUMA_THROWS)
endif()

# ---------------------------------------------------------------------
# Boost setup
# ---------------------------------------------------------------------
find_package(Boost REQUIRED)
target_link_libraries(hwmalloc PRIVATE Boost::boost)

# ---------------------------------------------------------------------
# MPI setup
# ---------------------------------------------------------------------
include(hwmalloc_mpi)
if (HWMALLOC_WITH_MPI)
    target_link_libraries(hwmalloc PRIVATE MPI::MPI_CXX)
endif()

# ---------------------------------------------------------------------
# UCX setup
# ---------------------------------------------------------------------
include(hwmalloc_ucx)
if (HWMALLOC_WITH_UCX)
    target_link_libraries(hwmalloc PRIVATE UCP::libucp)
endif()

# ---------------------------------------------------------------------
# Libfabric setup
# Can be simplified as this is copied from hpx and options obsolete
# but the config files with namespaces are very useful in big projects
# ---------------------------------------------------------------------
include(hwmalloc_libfabric)
if (HWMALLOC_WITH_LIBFABRIC)
    target_link_libraries(hwmalloc PRIVATE libfabric::libfabric)
endif()

# ---------------------------------------------------------------------
# GPU device support
# ---------------------------------------------------------------------
set(HWMALLOC_ENABLE_DEVICE OFF CACHE BOOL "True if GPU support shall be enabled")

# ---------------------------------------------------------------------
# include paths
# ---------------------------------------------------------------------
target_include_directories(hwmalloc PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

# ---------------------------------------------------------------------
# Generate hwmalloc config file
# ---------------------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/hwmalloc/config.hpp @ONLY)

# ---------------------------------------------------------------------
# main src subdir
# ---------------------------------------------------------------------
add_subdirectory(src)

# ---------------------------------------------------------------------
# testing
# ---------------------------------------------------------------------
set(HWMALLOC_WITH_TESTING OFF CACHE BOOL "True if tests shall be built")
if (HWMALLOC_WITH_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

# ---------------------------------------------------------------------
# install rules
# ---------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS hwmalloc
    EXPORT HWMALLOC-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT HWMALLOC-targets
  FILE HWMALLOC-targets.cmake
  NAMESPACE HWMALLOC::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/HWMALLOCConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HWMALLOCConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

write_basic_package_version_file(HWMALLOCConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HWMALLOCConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HWMALLOCConfigVersion.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/FindHWLOC.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake)
export(EXPORT HWMALLOC-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/HWMALLOC-targets.cmake")
