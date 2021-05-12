#pragma once

#include <hwmalloc/numa.hpp>
#include <hwmalloc/register.hpp>
#include <type_traits>
#include <boost/lockfree/stack.hpp>
#include <atomic>

namespace hwmalloc
{
namespace detail
{
template<typename Context>
class pool;

template<typename Context>
class segment
{
  public:
    using pool_type = pool<Context>;

    using region_type_cpu = decltype(hwmalloc::register_memory(*((Context*)0), nullptr, 0, cpu));
    static_assert(
        !std::is_copy_constructible<region_type_cpu>::value, "region is copy constructible");
    static_assert(
        std::is_move_constructible<region_type_cpu>::value, "region is not move constructible");

    using handle_type_cpu = typename region_type_cpu::handle_type;
    static_assert(std::is_default_constructible<handle_type_cpu>::value,
        "handle is not default constructible");
    static_assert(
        std::is_copy_constructible<handle_type_cpu>::value, "handle is not copy constructible");
    static_assert(std::is_copy_assignable<handle_type_cpu>::value, "handle is not copy assignable");

    struct block
    {
        segment*        m_segment = nullptr;
        void*           m_ptr = nullptr;
        handle_type_cpu m_handle_cpu;

        //void release() const noexcept
        //{
        //    while (!m_segment->m_freed_stack.push(*this)) {}
        //    ++m_segment->m_num_freed;
        //}
    };

    struct allocation_holder
    {
        numa_tools::allocation m;
        ~allocation_holder() noexcept
        {
            std::cout << "  allocation destructor" << std::endl;
            hwmalloc::numa().free(m);
        }
    };

  private:
    using stack_type = boost::lockfree::stack<block, boost::lockfree::fixed_sized<true>>;

    pool_type*        m_pool;
    std::size_t       m_block_size;
    std::size_t       m_num_blocks;
    allocation_holder m_allocation;
    region_type_cpu   m_region_cpu;
    stack_type        m_freed_stack;
    std::atomic<long> m_num_freed;
    //std::mutex m_mutex;

  public:
    template<typename Stack>
    segment(pool_type* pool, region_type_cpu&& region, numa_tools::allocation alloc,
        std::size_t block_size, Stack& free_stack)
    : m_pool{pool}
    , m_block_size{block_size}
    , m_num_blocks{alloc.size / block_size}
    , m_allocation{alloc}
    , m_region_cpu{std::move(region)}
    , m_freed_stack(m_num_blocks)
    , m_num_freed(0)
    {
        char* origin = (char*)m_allocation.m.ptr;
        //for (std::size_t i = 0; i < m_num_blocks; ++i)
        for (std::size_t i = m_num_blocks; i > 0; --i)
        {
            block b{this, origin + (i - 1) * block_size,
                m_region_cpu.get_handle((i - 1) * block_size, block_size)};
            while (!free_stack.push(b)) {}
        }
    }

    segment(segment const&) = delete;
    segment(segment&&) = delete;

    std::size_t block_size() const noexcept { return m_block_size; }
    std::size_t capacity() const noexcept { return m_num_blocks; }
    std::size_t numa_node() const noexcept { return m_allocation.m.node; }
    pool_type*  get_pool() const noexcept { return m_pool; }

    bool is_empty() const noexcept
    {
        return static_cast<std::size_t>(m_num_freed.load()) == m_num_blocks;
    }

    template<typename Stack>
    std::size_t collect(Stack& stack)
    {
        std::cout << "collecting" << std::endl;
        const auto consumed = m_freed_stack.consume_all([&stack](block const& b) {
            //std::cout << "  pushing block " << b.m_ptr << std::endl;
            while (!stack.push(b)) {}
        });
        m_num_freed.fetch_sub(consumed);
        return consumed;
    }

    void free(block const& b)
    {
        while (!m_freed_stack.push(b)) {}
        ++m_num_freed;
    }
};

} // namespace detail
} // namespace hwmalloc
