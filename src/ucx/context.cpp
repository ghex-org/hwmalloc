/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/ucx/context.hpp>

namespace hwmalloc
{
namespace ucx
{
region
register_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size);
}

} // namespace ucx
} // namespace hwmalloc
