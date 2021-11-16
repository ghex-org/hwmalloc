/*
 * ghex-org
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/device.hpp>
#include <hwmalloc/log.hpp>
#include <cstdint>
#include <hip/hip_runtime.h>
#include <stdexcept>
#include <string>
#define HWMALLOC_CHECK_HIP_RESULT(x)                                                               \
    if (x != hipSuccess)                                                                           \
        throw std::runtime_error("hwmalloc error: HIP Call failed " + std::string(#x) + " (" +     \
                                 std::string(hipGetErrorString(x)) + ") in " +                     \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));

namespace hwmalloc
{
int
get_num_devices()
{
    int n;
    HWMALLOC_CHECK_HIP_RESULT(hipGetDeviceCount(&n));
    return n;
}

int
get_device_id()
{
    int id;
    HWMALLOC_CHECK_HIP_RESULT(hipGetDevice(&id));
    return id;
}

void
set_device_id(int id)
{
    HWMALLOC_CHECK_HIP_RESULT(hipSetDevice(id));
}

void*
device_malloc(std::size_t size)
{
    void* ptr;
    HWMALLOC_CHECK_HIP_RESULT(hipMalloc(&ptr, size));
    HWMALLOC_LOG("allocating", size, "bytes using hipMalloc on device", get_device_id(), ":",
        (std::uintptr_t)a.ptr);
    return ptr;
}

void
device_free(void* ptr) noexcept
{
    HWMALLOC_LOG("freeing    using hipFree on device", get_device_id(), ":", (std::uintptr_t)ptr);
    hipFree(ptr);
}

void
memcpy_to_device(void* dst, void const* src, std::size_t count)
{
    hipStream_t stream;
    HWMALLOC_CHECK_HIP_RESULT(hipStreamCreateWithFlags(&stream, hipStreamNonBlocking));
    HWMALLOC_CHECK_HIP_RESULT(hipMemcpyAsync(dst, src, count, hipMemcpyHostToDevice, stream));
    hipEvent_t done;
    HWMALLOC_CHECK_HIP_RESULT(
        hipEventCreateWithFlags(&done, /*hipEventBlockingSync |*/ hipEventDisableTiming));
    HWMALLOC_CHECK_HIP_RESULT(hipEventRecord(done, stream));
    HWMALLOC_CHECK_HIP_RESULT(hipEventSynchronize(done));
    HWMALLOC_CHECK_HIP_RESULT(hipEventDestroy(done));
}

void
memcpy_to_host(void* dst, void const* src, std::size_t count)
{
    hipStream_t stream;
    HWMALLOC_CHECK_HIP_RESULT(hipStreamCreateWithFlags(&stream, hipStreamNonBlocking));
    HWMALLOC_CHECK_HIP_RESULT(hipMemcpyAsync(dst, src, count, hipMemcpyDeviceToHost, stream));
    hipEvent_t done;
    HWMALLOC_CHECK_HIP_RESULT(
        hipEventCreateWithFlags(&done, /*hipEventBlockingSync |*/ hipEventDisableTiming));
    HWMALLOC_CHECK_HIP_RESULT(hipEventRecord(done, stream));
    HWMALLOC_CHECK_HIP_RESULT(hipEventSynchronize(done));
    HWMALLOC_CHECK_HIP_RESULT(hipEventDestroy(done));
}

} // namespace hwmalloc
