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
    return std::memset(std::malloc(size), 0, size);
}

void
device_free(void* ptr) noexcept
{
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
