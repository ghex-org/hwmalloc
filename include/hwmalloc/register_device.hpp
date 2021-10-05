/*
 * ghex-org
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/register.hpp>

namespace hwmalloc
{
namespace detail
{
// default implementation: call normal registration
template<class Context>
constexpr auto
register_device_memory(Context&& c, void* ptr, std::size_t size) noexcept(
    noexcept(hwmalloc::register_memory(std::forward<Context>(c), ptr, size)))
    -> decltype(hwmalloc::register_memory(std::forward<Context>(c), ptr, size))
{
    return hwmalloc::register_memory(std::forward<Context>(c), ptr, size);
}

struct register_device_fn
{
    template<typename Context>
    constexpr auto operator()(Context&& c, void* ptr, std::size_t size) const
        noexcept(noexcept(register_device_memory(std::forward<Context>(c), ptr, size)))
            -> decltype(register_device_memory(std::forward<Context>(c), ptr, size))
    {
        return register_device_memory(std::forward<Context>(c), ptr, size);
    }
};
} // namespace detail

namespace
{
constexpr auto const& register_device_memory = static_const_v<detail::register_device_fn>;
}

} // namespace hwmalloc
