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

  private:
    Context*                                m_context;
    std::size_t                             m_block_size;
    std::size_t                             m_segment_size;
    bool                                    m_never_free;
    std::vector<std::unique_ptr<pool_type>> m_pools;

  public:
    fixed_size_heap(
        Context* context, std::size_t block_size, std::size_t segment_size, bool never_free)
    : m_context(context)
    , m_block_size(block_size)
    , m_segment_size(segment_size)
    , m_never_free(never_free)
    , m_pools(numa().num_host_nodes())
    {
        for (unsigned int i = 0; i < numa().num_host_nodes(); ++i)
            m_pools[i] = std::make_unique<pool_type>(
                m_context, m_block_size, m_segment_size, i, m_never_free);
    }

    fixed_size_heap(fixed_size_heap const&) = delete;
    fixed_size_heap(fixed_size_heap&&) = default;

    block_type allocate(std::size_t numa_node) { return m_pools[numa_node]->allocate(); }

    void free(block_type const& b) { b.release(); }
};

} // namespace detail
} // namespace hwmalloc
