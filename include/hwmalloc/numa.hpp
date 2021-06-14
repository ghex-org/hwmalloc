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

#include <vector>

#ifdef HWMALLOC_NUMA_THROWS
#define HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT
#else
#define HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT noexcept
#endif

namespace hwmalloc
{
// Query numa memory regions and explicitely allocate on specific regions.
// This class is a thin wrapper over some of libnuma's functionality.
class numa_tools
{
  public:
    using index_type = std::size_t;
    using size_type = std::size_t;

    struct allocation
    {
        void* const      ptr = nullptr;
        size_type const  size = 0u;
        index_type const node = 0u;
        bool const       use_numa_free = true;

        operator bool() const noexcept { return (bool)ptr; }
    };

  private:
    static bool      is_initialized_;
    static size_type page_size_;

  public:
    static bool      is_initialized() noexcept { return numa_tools::is_initialized_; }
    static size_type page_size() noexcept { return numa_tools::page_size_; }

  private:
    std::vector<index_type> m_host_nodes;
    std::vector<index_type> m_device_nodes;
    std::vector<index_type> m_allowed_nodes;

  private:
    numa_tools() HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT;
    friend numa_tools make_numa_tools() HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT;

  public:
    ~numa_tools() noexcept;

  public:
    const auto& host_nodes() const noexcept { return m_host_nodes; }
    const auto& device_nodes() const noexcept { return m_device_nodes; }
    const auto& allowed_nodes() const noexcept { return m_allowed_nodes; }
    index_type  num_host_nodes() const noexcept { return m_host_nodes.size(); }
    index_type  num_device_nodes() const noexcept { return m_device_nodes.size(); }
    index_type  num_allowed_nodes() const noexcept { return m_allowed_nodes.size(); }
    index_type  preferred_node() const noexcept;
    bool        can_allocate_on(index_type node) const noexcept;
    allocation  allocate(size_type num_pages) const noexcept;
    allocation  allocate(size_type num_pages, index_type node) const noexcept;
    allocation  allocate_malloc(size_type num_pages) const noexcept;
    void        free(allocation const& a) const noexcept;

  private:
    void       discover_nodes() noexcept;
    index_type get_node(void* ptr) const noexcept;
};

const numa_tools& numa() noexcept;

} // namespace hwmalloc
