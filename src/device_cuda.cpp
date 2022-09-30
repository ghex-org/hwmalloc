/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/device.hpp>
#include <hwmalloc/log.hpp>
#include <cstdint>
#include <iomanip>
#include <cuda_runtime.h>
#include <stdexcept>
#include <string>
#define HWMALLOC_CHECK_CUDA_RESULT(x)                                                              \
    if (x != cudaSuccess)                                                                          \
        throw std::runtime_error("hwmalloc error: CUDA Call failed " + std::string(#x) + " (" +    \
                                 std::string(cudaGetErrorString(x)) + ") in " +                    \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));

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

#ifdef HWMALLOC_ENABLE_LOGGING
    std::stringstream tmp;
    tmp << std::right << "0x" << std::setfill('0') << std::setw(12) << std::noshowbase
            << std::hex << reinterpret_cast<uintptr_t>(ptr);
    HWMALLOC_LOG("allocating", size, "bytes using cudaMalloc on device", get_device_id(), ":", tmp.str());
#endif
    return ptr;
}

void
device_free(void* ptr) noexcept
{
    HWMALLOC_LOG("freeing    using cudaFree on device", get_device_id(), ":", (std::uintptr_t)ptr);
    cudaFree(ptr);
}

void
memcpy_to_device(void* dst, void const* src, std::size_t count)
{
    cudaStream_t stream;
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    HWMALLOC_CHECK_CUDA_RESULT(cudaMemcpyAsync(dst, src, count, cudaMemcpyHostToDevice, stream));
    cudaEvent_t done;
    HWMALLOC_CHECK_CUDA_RESULT(
        cudaEventCreateWithFlags(&done, /*cudaEventBlockingSync |*/ cudaEventDisableTiming));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventRecord(done, stream));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventSynchronize(done));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventDestroy(done));
}

void
memcpy_to_host(void* dst, void const* src, std::size_t count)
{
    cudaStream_t stream;
    HWMALLOC_CHECK_CUDA_RESULT(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    HWMALLOC_CHECK_CUDA_RESULT(cudaMemcpyAsync(dst, src, count, cudaMemcpyDeviceToHost, stream));
    cudaEvent_t done;
    HWMALLOC_CHECK_CUDA_RESULT(
        cudaEventCreateWithFlags(&done, /*cudaEventBlockingSync |*/ cudaEventDisableTiming));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventRecord(done, stream));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventSynchronize(done));
    HWMALLOC_CHECK_CUDA_RESULT(cudaEventDestroy(done));
}

} // namespace hwmalloc
