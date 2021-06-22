enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    #GIT_TAG        release-1.10.0
    GIT_TAG        master
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
    target_link_libraries(${t} PRIVATE gtest_main)
    target_link_libraries(${t} PRIVATE hwmalloc)
    target_link_libraries(${t} PRIVATE Boost::boost)
    add_test(NAME ${t} COMMAND $<TARGET_FILE:${t}>)
endfunction()

reg_test(test_numa)
reg_test(test_ptr)
reg_test(test_segment)

find_package(OpenMP REQUIRED)
reg_test(test_omp)
target_link_libraries(test_omp OpenMP::OpenMP_CXX NUMA::libnuma)

if (HWMALLOC_ENABLE_DEVICE)
reg_test(test_device)
endif()

if (HWMALLOC_WITH_MPI OR HWMALLOC_WITH_UCX)
    add_subdirectory(mpi_runner)
endif()

if (HWMALLOC_WITH_MPI)
    add_subdirectory(mpi)
endif()

if (HWMALLOC_WITH_UCX)
    add_subdirectory(ucx)
endif()
