if (NUMA_LIBRARY)
    target_sources(hwmalloc PRIVATE numa.cpp)
else()
    target_sources(hwmalloc PRIVATE numa_stub.cpp)
endif()

if (HWMALLOC_ENABLE_LOGGING)
    target_sources(hwmalloc PRIVATE log.cpp)
endif()

if (HWMALLOC_ENABLE_DEVICE)
    set(HWMALLOC_DEVICE_RUNTIME "cuda" CACHE STRING "Choose the type of the gpu runtime.")
    set_property(CACHE HWMALLOC_DEVICE_RUNTIME PROPERTY STRINGS "cuda" "hip" "emulate")

    string(TOUPPER ${HWMALLOC_DEVICE_RUNTIME} HWMALLOC_DEVICE_RUNTIME_U)
    set(HWMALLOC_DEVICE_RUNTIME_ ${HWMALLOC_DEVICE_RUNTIME_U} CACHE INTERNAL "" FORCE)
    set(HWMALLOC_DEVICE "HWMALLOC_DEVICE_${HWMALLOC_DEVICE_RUNTIME_}" CACHE INTERNAL "" FORCE)

    if (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "cuda")
        find_package(CUDAToolkit REQUIRED)
        target_link_libraries(hwmalloc PRIVATE CUDA::cudart)
    elseif (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "hip")
        find_package(hip REQUIRED)
        target_link_libraries(hwmalloc PRIVATE hip::device)
        # workaround for clang-hip
        target_compile_options(hwmalloc PRIVATE $<BUILD_INTERFACE:-std=c++14>)
    endif()

    target_sources(hwmalloc PRIVATE device_${HWMALLOC_DEVICE_RUNTIME}.cpp)
else()
    set(HWMALLOC_DEVICE "HWMALLOC_DEVICE_NONE" CACHE INTERNAL "" FORCE)
    target_sources(hwmalloc PRIVATE device_none.cpp)
endif()
