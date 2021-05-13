target_sources(hwmalloc PRIVATE numa.cpp)

if (HWMALLOC_ENABLE_DEVICE)
    set(HWMALLOC_DEVICE_RUNTIME "cuda" CACHE STRING "Choose the type of the gpu runtime.")
    set_property(CACHE HWMALLOC_DEVICE_RUNTIME PROPERTY STRINGS "cuda" "hip")

    if (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "cuda")
        find_package(CUDAToolkit REQUIRED)
        target_link_libraries(hwmalloc PRIVATE CUDA::cudart)
    endif()

    target_sources(hwmalloc PRIVATE device_${HWMALLOC_DEVICE_RUNTIME}.cpp)

endif()
