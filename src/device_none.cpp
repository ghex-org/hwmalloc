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

namespace hwmalloc
{
int
get_num_devices()
{
    return 0;
}

int
get_device_id()
{
    return 0;
}

void
set_device_id(int)
{
}

void*
device_malloc(std::size_t)
{
    return nullptr;
}

void
device_free(void*) noexcept
{
}

void
memcpy_to_device(void*, void const*, std::size_t)
{
}

void
memcpy_to_host(void*, void const*, std::size_t)
{
}

} // namespace hwmalloc
