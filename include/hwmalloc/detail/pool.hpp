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

    void add_segment()
    {
        std::cout << "adding segment" << std::endl;
        auto a =
            check_allocation(numa().allocate(num_pages(m_segment_size), m_numa_node), m_numa_node);
        auto s = std::make_unique<segment_type>(this,
            hwmalloc::register_memory(*m_context, a.ptr, a.size, cpu), a, m_block_size,
            m_free_stack);
        m_segments[s.get()] = std::move(s);
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
        //add_segment();
    }

    auto allocate()
    {
        block_type b;
        if (m_free_stack.pop(b)) return b;
        std::lock_guard<std::mutex> lock(m_mutex);
        {
            if (m_free_stack.pop(b)) return b;
            for (auto& kvp : m_segments) kvp.first->collect(m_free_stack);
            if (m_free_stack.pop(b)) return b;
            unsigned int counter = 0;
            while (!m_free_stack.pop(b))
            {
                // add segments every 2nd iteration
                if (counter % 2 == 0) add_segment();
            }
            return b;
        }
    }

    void free(block_type const& b)
    {
        b.m_segment->free(b);
        if (!m_never_free && b.m_segment->is_empty())
        {
            std::cout << "segment is empty" << std::endl;
            std::lock_guard<std::mutex> lock(m_mutex);
            if (b.m_segment->is_empty() /*&& m_segments.size()>0*/)
            {
                std::cout << "releasing segment" << std::endl;
                m_segments[b.m_segment].reset();
            }
        }
    }
};

} // namespace detail
} // namespace hwmalloc
