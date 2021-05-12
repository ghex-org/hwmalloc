#include <gtest/gtest.h>

#include <hwmalloc/detail/segment.hpp>
#include <hwmalloc/detail/pool.hpp>
#include <hwmalloc/detail/fixed_size_heap.hpp>
#include <hwmalloc/heap.hpp>

#include <thread>

struct context
{
    int m = 42;
    context() { std::cout << "context constructor" << std::endl; }
    ~context() { std::cout << "context destructor" << std::endl; }

    struct region
    {
        struct handle_type
        {
            void* ptr;
        };

        void* ptr = nullptr;

        region(void* p) noexcept
        : ptr{p}
        {
        }

        region(region const&) = delete;

        region(region&& other) noexcept
        : ptr{std::exchange(other.ptr, nullptr)}
        {
        }

        ~region()
        {
            if (ptr) std::cout << "    region destructor" << std::endl;
        }

        handle_type get_handle(std::size_t offset, std::size_t /*size*/) const noexcept
        {
            return {(void*)((char*)ptr + offset)};
        }
    };
};

auto
register_memory(context&, void* ptr, std::size_t, hwmalloc::cpu_t)
{
    return context::region{ptr};
}

TEST(segment, construction)
{
    using segment_t = hwmalloc::detail::segment<context>;
    using block_t = segment_t::block;

    context c;

    auto a = hwmalloc::numa().allocate(1, 0);
    auto r = hwmalloc::register_memory(c, a.ptr, a.size, hwmalloc::cpu);

    boost::lockfree::stack<block_t> free_stack(256);

    segment_t s(nullptr, std::move(r), a, sizeof(int), free_stack);

    while (true)
    {
        block_t x;
        if (!free_stack.pop(x)) break;
        else
        {
            std::cout << x.m_ptr << std::endl;
            //x.release();
            x.m_segment->free(x);
        }
    }
    EXPECT_TRUE(s.is_empty());

    s.collect(free_stack);
}

TEST(pool, construction)
{
    using pool_t = hwmalloc::detail::pool<context>;
    //using block_t = pool_t::block_type;

    context c;

    pool_t p(&c, 8, hwmalloc::numa().page_size(), 0, false);

    for (unsigned int i = 0; i < 512; ++i)
    {
        auto b = p.allocate();
        std::cout << b.m_ptr << std::endl;
        if (i % 2 == 0) p.free(b);
    }

    {
        auto b = p.allocate();
        std::cout << b.m_ptr << std::endl;
        p.free(b);
    }
}

TEST(fixed_size_heap, construction)
{
    using heap_t = hwmalloc::detail::fixed_size_heap<context>;

    context c;

    heap_t h(&c, 8, hwmalloc::numa().page_size(), false);

    for (unsigned int i = 0; i < 512; ++i)
    {
        auto b = h.allocate(0);
        std::cout << b.m_ptr << std::endl;
        if (i % 2 == 0) h.free(b);
    }

    {
        auto b = h.allocate(0);
        std::cout << b.m_ptr << std::endl;
        h.free(b);
    }
}

TEST(heap, construction)
{
    using heap_t = hwmalloc::heap<context>;

    context c;

    heap_t h(&c);

    auto ptr = h.allocate(1, 0);
    std::cout << ptr.get() << std::endl;
    h.free(ptr);

    //{
    //auto ub = h.allocate_unique(65536, 0);
    //}
    //std::cout << "after unique ptr destructor" << std::endl;
}

TEST(heap, allocator)
{
    using heap_t = hwmalloc::heap<context>;

    context c;

    heap_t h(&c);

    std::vector<double, heap_t::allocator_type<double>> vec(h.template get_allocator<double>(0));

    vec.resize(500);
}
