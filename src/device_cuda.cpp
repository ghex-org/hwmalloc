/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/device.hpp>
#include <cuda_runtime.h>

//#ifdef NDEBUG
//#define HWMALLOC_CHECK_CUDA_RESULT(x) x;
//#else

#include <stdexcept>
#include <string>
#define HWMALLOC_CHECK_CUDA_RESULT(x)                                                              \
    if (x != cudaSuccess)                                                                          \
        throw std::runtime_error("hwmalloc error: CUDA Call failed " + std::string(#x) + " (" +    \
                                 std::string(cudaGetErrorString(x)) + ") in " +                    \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));

//#endif

namespace hwmalloc
{
int
get_num_devices()
{
    int n;
    HWMALLOC_CHECK_CUDA_RESULT(cudaGetDeviceCount(&n));
    return n;
}

int
get_device_id()
{
    int id;
    HWMALLOC_CHECK_CUDA_RESULT(cudaGetDevice(&id));
    return id;
}

void
set_device_id(int id)
{
    HWMALLOC_CHECK_CUDA_RESULT(cudaSetDevice(id));
}

void*
device_malloc(std::size_t size)
{
    void* ptr;
    HWMALLOC_CHECK_CUDA_RESULT(cudaMalloc(&ptr, size));
    return ptr;
}

void
device_free(void* ptr) noexcept
{
    cudaFree(ptr);
}

void
memcpy_to_device(void* dst, void const* src, std::size_t count)
{
    cudaStream_t stream;
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    HWMALLOC_CHECK_CUDA_RESULT(cudaMemcpyAsync(dst, src, count, cudaMemcpyHostToDevice, stream));
    while (cudaSuccess != cudaStreamQuery(stream)) {}
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamDestroy(stream));
}

void
memcpy_to_host(void* dst, void const* src, std::size_t count)
{
    cudaStream_t stream;
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    HWMALLOC_CHECK_CUDA_RESULT(cudaMemcpyAsync(dst, src, count, cudaMemcpyDeviceToHost, stream));
    while (cudaSuccess != cudaStreamQuery(stream)) {}
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamDestroy(stream));
}

} // namespace hwmalloc
