target_sources(hwmalloc PRIVATE heap_config.cpp)

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
        message(STATUS "CMAKE_HIP_ARCHITECTURES = ${CMAKE_HIP_ARCHITECTURES}")
        add_library(hip_gpu_code OBJECT device_hip.cpp)
        set_source_files_properties(device_hip.cpp PROPERTIES
            LANGUAGE HIP
        )
        target_include_directories(hip_gpu_code PRIVATE
            "$<TARGET_PROPERTY:hwmalloc,INTERFACE_INCLUDE_DIRECTORIES>"
        )
    endif()
    
    if (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "cuda")
        target_sources(hwmalloc PRIVATE device_${HWMALLOC_DEVICE_RUNTIME}.cpp)
    elseif (${HWMALLOC_DEVICE_RUNTIME} STREQUAL "hip")
        target_link_libraries(hwmalloc PRIVATE hip_gpu_code)
    endif()
else()
    set(HWMALLOC_DEVICE "HWMALLOC_DEVICE_NONE" CACHE INTERNAL "" FORCE)
    target_sources(hwmalloc PRIVATE device_none.cpp)
endif()
