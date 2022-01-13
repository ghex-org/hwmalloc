/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/detail/pool.hpp>
#include <vector>

namespace hwmalloc
{
namespace detail
{
template<typename Context>
class fixed_size_heap
{
  public:
    using pool_type = pool<Context>;
    using block_type = typename pool_type::block_type;
    using handle_type = typename pool_type::handle_type;

  private:
    Context*                                m_context;
    std::size_t                             m_block_size;
    std::size_t                             m_segment_size;
    bool                                    m_never_free;
    std::vector<std::unique_ptr<pool_type>> m_pools;
#if HWMALLOC_ENABLE_DEVICE
    std::size_t                             m_num_devices;
    std::vector<std::unique_ptr<pool_type>> m_device_pools;
#endif

  public:
    fixed_size_heap(
        Context* context, std::size_t block_size, std::size_t segment_size, bool never_free)
    : m_context(context)
    , m_block_size(block_size)
    , m_segment_size(segment_size)
    , m_never_free(never_free)
    , m_pools(numa().num_host_nodes())
#if HWMALLOC_ENABLE_DEVICE
    , m_num_devices{(std::size_t)get_num_devices()}
    , m_device_pools(numa().num_host_nodes() * m_num_devices)
#endif
    {
        for (unsigned int i = 0; i < numa().num_host_nodes(); ++i)
        {
            m_pools[i] = std::make_unique<pool_type>(
                m_context, m_block_size, m_segment_size, i, m_never_free);
#if HWMALLOC_ENABLE_DEVICE
            for (unsigned int j = 0; j < m_num_devices; ++j)
            {
                m_device_pools[i * m_num_devices + j] = std::make_unique<pool_type>(
                    m_context, m_block_size, m_segment_size, i, (int)j, m_never_free);
            }
#endif
        }
    }

    fixed_size_heap(fixed_size_heap const&) = delete;
    fixed_size_heap(fixed_size_heap&&) = default;

    block_type allocate(std::size_t numa_node) { return m_pools[numa_node]->allocate(); }

    block_type register_user_allocation(void * const ptr, std::size_t const size)
    {
        return m_pools[0]->register_user_allocation(ptr, size);
    }

#if HWMALLOC_ENABLE_DEVICE
    block_type allocate(std::size_t numa_node, int device_id)
    {
        return m_device_pools[numa_node * m_num_devices + device_id]->allocate();
    }
#endif

    void free(block_type const& b) { b.release(); }
};

} // namespace detail
} // namespace hwmalloc
