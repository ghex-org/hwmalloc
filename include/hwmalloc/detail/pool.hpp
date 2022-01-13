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

#include <hwmalloc/detail/segment.hpp>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <stdexcept>

namespace hwmalloc
{
namespace detail
{
template<typename Context>
class pool
{
  public:
    using segment_type = segment<Context>;
    using block_type = typename segment_type::block;
    using handle_type = typename segment_type::handle_type;
    using stack_type = boost::lockfree::stack<block_type>;
    using segment_map = std::unordered_map<segment_type*, std::unique_ptr<segment_type>>;

  private:
    static std::size_t num_pages(std::size_t segment_size) noexcept
    {
        auto x = (segment_size + numa().page_size() - 1) / numa().page_size();
        return x;
    }

    static auto check_allocation(numa_tools::allocation const& a, std::size_t expected_numa_node)
    {
        if (!a) { throw std::runtime_error("could not allocate system memory"); }
        else if (a.node != expected_numa_node)
        {
            numa().free(a);
            throw std::runtime_error("could not allocate on requested numa node");
        }
        return a;
    }

  private:
    Context*    m_context;
    std::size_t m_block_size;
    std::size_t m_segment_size;
    std::size_t m_numa_node;
    bool        m_never_free;
    stack_type  m_free_stack;
    segment_map m_segments;
    std::mutex  m_mutex;
    int         m_device_id = 0;
    bool        m_allocate_on_device = false;

    void add_segment()
    {
        auto a =
            check_allocation(numa().allocate(num_pages(m_segment_size), m_numa_node), m_numa_node);
#if HWMALLOC_ENABLE_DEVICE
        if (m_allocate_on_device)
        {
            const auto tmp = get_device_id();
            set_device_id(m_device_id);
            void* device_ptr = device_malloc(a.size);

            auto s = std::make_unique<segment_type>(this,
                hwmalloc::register_memory(*m_context, a.ptr, a.size), a,
                hwmalloc::register_device_memory(*m_context, device_ptr, a.size), device_ptr,
                m_block_size, m_free_stack);
            m_segments[s.get()] = std::move(s);
            set_device_id(tmp);
        }
        else
#endif
        {
            auto s = std::make_unique<segment_type>(this,
                hwmalloc::register_memory(*m_context, a.ptr, a.size), a, m_block_size,
                m_free_stack);
            m_segments[s.get()] = std::move(s);
        }
    }

  public:
    pool(Context* context, std::size_t block_size, std::size_t segment_size, std::size_t numa_node,
        bool never_free)
    : m_context{context}
    , m_block_size{block_size}
    , m_segment_size{segment_size}
    , m_numa_node{numa_node}
    , m_never_free{never_free}
    , m_free_stack(segment_size / block_size)
    {
    }

#if HWMALLOC_ENABLE_DEVICE
    pool(Context* context, std::size_t block_size, std::size_t segment_size, std::size_t numa_node,
        int device_id, bool never_free)
    : pool(context, block_size, segment_size, numa_node, never_free)
    {
        m_device_id = device_id;
        m_allocate_on_device = true;
    }
#endif

    auto allocate()
    {
        block_type b;
        if (m_free_stack.pop(b)) return b;
        while (!m_mutex.try_lock())
            if (m_free_stack.pop(b))
            {
                m_mutex.unlock();
                return b;
            }
        for (auto& kvp : m_segments) kvp.first->collect(m_free_stack);
        if (m_free_stack.pop(b))
        {
            m_mutex.unlock();
            return b;
        }
        unsigned int counter = 0;
        while (!m_free_stack.pop(b))
        {
            // add segments every 2nd iteration
            if (counter++ % 2 == 0) add_segment();
        }
        m_mutex.unlock();
        return b;
    }

    auto register_user_allocation(void * const ptr, std::size_t const size)
    {
        auto region = hwmalloc::register_memory(*m_context, ptr, size, hwmalloc::registration_flags::memory_user);
        block_type b = { nullptr, ptr, region.get_handle(0, size) };
        return b;
    }

    void free(block_type const& b)
    {
        b.m_segment->free(b);
        if (!m_never_free && b.m_segment->is_empty())
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (b.m_segment->is_empty() && m_segments.size() > 1) m_segments.erase(b.m_segment);
        }
    }
};

} // namespace detail
} // namespace hwmalloc
