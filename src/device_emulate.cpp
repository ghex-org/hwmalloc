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
#include <cstdlib>
#include <cstring>

namespace hwmalloc
{
int
get_num_devices()
{
    return 1;
}

int
get_device_id()
{
    return 0;
}

void
set_device_id(int /*id*/)
{
}

void*
device_malloc(std::size_t size)
{
    auto ptr = std::memset(std::malloc(size), 0, size);
    HWMALLOC_LOG("allocating", size, "bytes using emulate (std::malloc):", (std::uintptr_t)ptr);
    return ptr;
}

void
device_free(void* ptr) noexcept
{
    HWMALLOC_LOG("freeing    using emulate (std::free):", (std::uintptr_t)ptr);
    std::free(ptr);
}

void
memcpy_to_device(void* dst, void const* src, std::size_t count)
{
    std::memcpy(dst, src, count);
}

void
memcpy_to_host(void* dst, void const* src, std::size_t count)
{
    std::memcpy(dst, src, count);
}

} // namespace hwmalloc
