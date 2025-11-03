enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    #GIT_TAG        release-1.10.0
    GIT_TAG        main
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    # https://github.com/google/googletest/issues/2429
    add_library(GTest::gtest ALIAS gtest)
endif()

function(reg_test t)
    add_executable(${t} ${t}.cpp)
    hwmalloc_target_compile_options(${t})
    target_link_libraries(${t} PRIVATE gtest_main)
    target_link_libraries(${t} PRIVATE hwmalloc)
    target_link_libraries(${t} PRIVATE Boost::boost)
    target_include_directories(${t} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME ${t} COMMAND $<TARGET_FILE:${t}>)
endfunction()

set(HWMALLOC_DISABLE_NUMA_TEST OFF CACHE BOOL "disable numa test (for docker images)")
if (NUMA_LIBRARY AND NOT HWMALLOC_DISABLE_NUMA_TEST)
    reg_test(test_numa)
    if (HWMALLOC_NUMA_FOR_LOCAL)
        target_compile_definitions(test_numa PRIVATE HWMALLOC_NUMA_FOR_LOCAL)
    endif()
endif()
reg_test(test_ptr)
reg_test(test_segment)
reg_test(test_heap_config)
reg_test(test_heap_config_default)
reg_test(test_heap_config_invalid)

if (NUMA_LIBRARY)
find_package(OpenMP REQUIRED)
reg_test(test_omp)
target_link_libraries(test_omp PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(test_omp PRIVATE NUMA::libnuma)
endif()

if (HWMALLOC_ENABLE_DEVICE)
reg_test(test_device)
endif()
