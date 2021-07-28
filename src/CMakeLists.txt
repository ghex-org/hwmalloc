target_sources(hwmalloc PRIVATE numa.cpp)

if (HWMALLOC_ENABLE_DEVICE)
    set(HWMALLOC_DEVICE_RUNTIME "cuda" CACHE STRING "Choose the type of the gpu runtime.")
    set_property(CACHE HWMALLOC_DEVICE_RUNTIME PROPERTY STRINGS "cuda" "hip" "emulate")

    string(TOUPPER ${HWMALLOC_DEVICE_RUNTIME} HWMALLOC_DEVICE_RUNTIME_U)
    set(HWMALLOC_DEVICE_RUNTIME_ ${HWMALLOC_DEVICE_RUNTIME_U} CACHE INTERNAL "" FORCE)
    set(HWMALLOC_DEVICE "HWMALLOC_DEVICE_${HWMALLOC_DEVICE_RUNTIME_}" CACHE INTERNAL "" FORCE)

    if (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "cuda")
        find_package(CUDAToolkit REQUIRED)
        target_link_libraries(hwmalloc PRIVATE CUDA::cudart)
    else()
        # link to hip
    endif()

    target_sources(hwmalloc PRIVATE device_${HWMALLOC_DEVICE_RUNTIME}.cpp)
endif()

#if(HWMALLOC_WITH_MPI)
#    add_subdirectory(mpi)
#endif()
#
#if(HWMALLOC_WITH_UCX)
#    add_subdirectory(ucx)
#endif()
