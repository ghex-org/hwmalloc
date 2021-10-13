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
    using region_type = typename detail::region_traits<Context>::region_type;
#if HWMALLOC_ENABLE_DEVICE
    using device_region_type = typename detail::region_traits<Context>::device_region_type;
#endif
    using block_type = block_t<Context>;
    using pointer = hw_void_ptr<block_type>;

    struct host_allocation
    {
        void* m_ptr;
        bool  m_delete;
        host_allocation(void* ptr, bool del) noexcept
        : m_ptr{ptr}
        , m_delete{del}
        {
        }
        host_allocation(host_allocation const&) noexcept = delete;
        host_allocation& operator=(host_allocation const&) noexcept = delete;

        host_allocation(host_allocation&& other) noexcept
        : m_ptr{std::exchange(other.m_ptr, nullptr)}
        , m_delete{other.m_delete}
        {
        }
        host_allocation& operator=(host_allocation&& other) noexcept
        {
            if (m_ptr && m_delete) std::free(m_ptr);
            m_ptr = std::exchange(other.m_ptr, nullptr);
            m_delete = other.m_delete;
            return *this;
        }
        ~host_allocation()
        {
            if (m_ptr && m_delete) std::free(m_ptr);
        }
    };

    //Context*                     m_context;
    host_allocation m_host_allocation;
    region_type     m_region;
#if HWMALLOC_ENABLE_DEVICE
    std::unique_ptr<device_region_type> m_device_region;
#endif

    user_allocation(Context* context, void* ptr, std::size_t size)
    //: m_context{context}
    : m_host_allocation{ptr, false}
    , m_region{hwmalloc::register_memory(*context, ptr, size)}
    {
    }

#if HWMALLOC_ENABLE_DEVICE
    user_allocation(Context* context, void* device_ptr, int device_id, std::size_t size)
    : m_host_allocation{std::malloc(size), true}
    , m_region{hwmalloc::register_memory(*context, m_host_allocation.m_ptr, size)}
    , m_device_region{std::make_unique<device_region_type>(
          hwmalloc::register_device_memory(*context, device_ptr, size))}
    {
    }

    user_allocation(Context* context, void* ptr, void* device_ptr, int device_id, std::size_t size)
    : m_host_allocation{ptr, false}
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
    delete m_user_allocation;
}

} // namespace detail
} // namespace hwmalloc
