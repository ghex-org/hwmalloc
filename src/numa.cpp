#include <hwmalloc/numa.hpp>
#include <numaif.h>
#include <numa.h>
#include <unistd.h>
#include <algorithm>

#ifdef HWMALLOC_NUMA_THROWS
#include <stdexcept>
#define HWMALLOC_NUMA_ERROR(MSG) throw std::runtime_error("libnuma error: " MSG)
#else
#include <iostream>
#define HWMALLOC_NUMA_ERROR(MSG) std::cerr << "libnuma error: " << MSG << "\n"
#endif

#include <iostream>

namespace hwmalloc
{
bool                  numa_tools::is_initialized_ = false;
numa_tools::size_type numa_tools::page_size_ = sysconf(_SC_PAGESIZE);

// construct the single instance
numa_tools::numa_tools() HWMALLOC_NUMA_CONDITIONAL_NOEXCEPT
{
    // initialize libnuma
    if (numa_available() < 0) HWMALLOC_NUMA_ERROR("could not initialize libnuma");
    else
        discover_nodes();
}

// detect host and device nodes
void
numa_tools::discover_nodes() noexcept
{
    // allocate a cpu mask
    auto cpu_mask_ptr = numa_allocate_cpumask();
    // get maximum number of nodes
    const auto num_nodes = numa_all_nodes_ptr->size;
    // loop over nodes
    for (index_type i = 0; i < num_nodes; ++i)
        // bit i is set if the calling process can access node i
        if (numa_bitmask_isbitset(numa_all_nodes_ptr, i))
        {
            // check whether the node is host or device by
            // converting node to a bitset of cpus associated with it
            numa_node_to_cpus(i, cpu_mask_ptr);
            // if there are any cpus, it is a host node
            if (numa_bitmask_weight(cpu_mask_ptr) > 0) m_host_nodes.push_back(i);
            else
                m_device_nodes.push_back(i);
        }
    numa_free_cpumask(cpu_mask_ptr);

    // find the nodes on which the current task can allocate
    const auto node_mask = numa_get_mems_allowed();
    for (index_type i = 0; i < node_mask->size; ++i)
        if (numa_bitmask_isbitset(node_mask, i)) m_allowed_nodes.push_back(i);

    is_initialized_ = true;
}

numa_tools::index_type
numa_tools::preferred_node() const noexcept
{
    return numa_preferred();
}

bool
numa_tools::can_allocate_on(index_type node) const noexcept
{
    auto it = std::lower_bound(m_allowed_nodes.cbegin(), m_allowed_nodes.cend(), node);
    if (it == m_allowed_nodes.cend()) return false;
    return *it == node;
}

numa_tools::allocation
numa_tools::allocate(size_type num_pages) const noexcept
{
    return allocate(num_pages, preferred_node());
}

numa_tools::allocation
numa_tools::allocate(size_type num_pages, index_type node) const noexcept
{
    if (!can_allocate_on(node)) return {};
    auto ptr = numa_alloc_onnode(num_pages * page_size_, node);
    if (!ptr) return {};
    return {ptr, num_pages * page_size_, node};
}

void
numa_tools::free(numa_tools::allocation const& a) const noexcept
{
    numa_free(a.ptr, a.size);
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
