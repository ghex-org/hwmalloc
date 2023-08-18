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

#include <hwmalloc/detail/region_traits.hpp>

namespace hwmalloc
{
namespace detail
{
template<typename Context>
class segment;

template<typename Context>
struct user_allocation;

template<typename Context>
struct block_t
{
    using region_traits_type = region_traits<Context>;
    using handle_type = typename region_traits_type::handle_type;
#if HWMALLOC_ENABLE_DEVICE
    using device_handle_type = typename region_traits_type::device_handle_type;
#endif
    using segment_type = segment<Context>;
    using user_allocation_type = user_allocation<Context>;

    segment_type*         m_segment = nullptr;
    user_allocation_type* m_user_allocation = nullptr;
    void*                 m_ptr = nullptr;
    handle_type           m_handle;
#if HWMALLOC_ENABLE_DEVICE
    void*              m_device_ptr = nullptr;
    device_handle_type m_device_handle = device_handle_type();
    int                m_device_id = 0;

    bool on_device() const noexcept { return m_device_ptr; }
#else
    bool on_device() const noexcept { return false; }
#endif

    void release_from_segment() const noexcept;
    void release_user_allocation() const noexcept;

    void release() const noexcept
    {
        if (m_segment) release_from_segment();
        else if (m_user_allocation)
            release_user_allocation();
    }
};

} // namespace detail
} // namespace hwmalloc
