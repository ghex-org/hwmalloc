/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/numa.hpp>
#include <hwmalloc/log.hpp>
#include <unistd.h>
#include <cstdlib>

namespace hwmalloc
{
bool                  numa_tools::is_initialized_ = false;
numa_tools::size_type numa_tools::page_size_ = sysconf(_SC_PAGESIZE);

// construct the single instance
numa_tools::numa_tools() HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT
{
    discover_nodes();
}

numa_tools::~numa_tools() noexcept {}

// create single-node stub without numa support
void
numa_tools::discover_nodes() noexcept
{
    m_local_nodes = node_map({0});
    is_initialized_ = true;
}

numa_tools::index_type
numa_tools::local_node() const noexcept
{
    return static_cast<index_type>(0);
}

bool
numa_tools::can_allocate_on(index_type node) const noexcept
{
    return local_nodes().count(node) > 0u;
}

numa_tools::allocation
numa_tools::allocate(size_type num_pages) const noexcept
{
    return allocate(num_pages, local_node());
}

numa_tools::allocation
numa_tools::allocate(size_type num_pages, index_type /*node*/) const noexcept
{
    if (num_pages == 0u) return {};
    return allocate_malloc(num_pages);
}

numa_tools::allocation
numa_tools::allocate_malloc(size_type num_pages) const noexcept
{
    void* ptr = std::calloc(num_pages, page_size_);
    //void* ptr = std::malloc(num_pages * page_size_);
    if (!ptr) return {};
    HWMALLOC_LOG("allocating", num_pages * page_size_,
        "bytes using std::malloc:", (std::uintptr_t)ptr);
    return {ptr, num_pages * page_size_, get_node(ptr), false};
}

numa_tools::index_type
numa_tools::get_node(void* /*ptr*/) const noexcept
{
    return static_cast<index_type>(0);
}

void
numa_tools::free(numa_tools::allocation const& a) const noexcept
{
    if (a)
    {
        HWMALLOC_LOG("freeing   ", a.size, "bytes using std::free:", (std::uintptr_t)a.ptr);
        std::free(a.ptr);
    }
}

// factory function
// only available from within this translation unit
numa_tools
make_numa_tools() HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT
{
    return {};
}

// global instance of numa_tools with internal linkage
// only available within this translation unit
namespace
{
const numa_tools numa_tools_ = make_numa_tools();
}

// access to single instance
const numa_tools&
numa() noexcept
{
    return numa_tools_;
}

} // namespace hwmalloc
