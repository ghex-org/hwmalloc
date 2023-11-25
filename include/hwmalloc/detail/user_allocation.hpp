/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/detail/block.hpp>
#include <hwmalloc/fancy_ptr/void_ptr.hpp>
#include <memory>
#include <cstdlib>

namespace hwmalloc
{
namespace detail
{
template<typename Context>
struct user_allocation
{
    using region_type = decltype(hwmalloc::register_memory(*((Context*)0), nullptr, 0u));
#if HWMALLOC_ENABLE_DEVICE
    using device_region_type = typename detail::region_traits<Context>::device_region_type;
#endif
    using block_type = block_t<Context>;
    using pointer = hw_void_ptr<block_type>;

    void*           m_host_ptr;
    bool            m_host_delete;
    region_type     m_region;
#if HWMALLOC_ENABLE_DEVICE
    std::unique_ptr<device_region_type> m_device_region;
#endif

    user_allocation(Context* context, void* ptr, std::size_t size)
        : m_host_ptr{ptr}
        , m_host_delete{false}
        , m_region{hwmalloc::register_memory(*context, ptr, size)}
    {
    }

#if HWMALLOC_ENABLE_DEVICE
    user_allocation(Context* context, void* device_ptr, int /*device_id*/, std::size_t size)
        : m_host_ptr{std::malloc(size)}
        , m_host_delete{true}
        , m_region{hwmalloc::register_memory(*context, m_host_ptr, size)}
        , m_device_region{std::make_unique<device_region_type>(
          hwmalloc::register_device_memory(*context, device_ptr, size))}
    {
    }

    user_allocation(Context* context, void* ptr, void* device_ptr, int /*device_id*/, std::size_t size)
        : m_host_ptr{ptr}
        , m_host_delete{false}
        , m_region{hwmalloc::register_memory(*context, ptr, size)}
        , m_device_region{std::make_unique<device_region_type>(
          hwmalloc::register_device_memory(*context, device_ptr, size))}
    {
    }
#endif
};

template<typename Context>
void
block_t<Context>::release_user_allocation() const noexcept
{
    // do we need to check m_user_ptr?
    m_handle.deregister();
    if (m_user_delete) {
        std::free(m_user_ptr);
    }
}

} // namespace detail
} // namespace hwmalloc
