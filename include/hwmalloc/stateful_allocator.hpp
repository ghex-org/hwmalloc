#pragma once

#include <hwmalloc/malloc.hpp>

namespace hwmalloc
{
template<typename T, memory_type M>
class stateful_allocator
{
  public:
    using this_type = stateful_allocator<T, M>;
    using value_type = T;
    using pointer = T*;
    using const_pointer = T const*;
    using void_pointer = void*;
    using const_void_pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    template<typename U>
    struct other_alloc
    {
        using other = stateful_allocator<U, M>;
    };
    template<typename U>
    using rebind = other_alloc<U>;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

  private:
    int m_memory_domain;

  public:
    stateful_allocator(int memory_domain = 0) noexcept
    : m_memory_domain{memory_domain}
    {
    }

    template<typename U>
    stateful_allocator(stateful_allocator<U, M> const& other) noexcept
    : m_memory_domain{other.m_memory_domain}
    {
    }

    template<typename U>
    stateful_allocator& operator=(stateful_allocator<U, M> const& other) noexcept
    {
        m_memory_domain = other.m_memory_domain;
        return *this;
    }

    template<typename U>
    void swap(stateful_allocator<U, M>& other) noexcept
    {
        const auto tmp = m_memory_domain;
        m_memory_domain = other.m_memory_domain;
        other.m_memory_domain = tmp;
    }

    stateful_allocator select_on_container_copy_construction() const noexcept
    {
        return {m_memory_domain};
    }

  public:
    pointer allocate(size_type n) //, const_void_pointer = const_void_pointer())
    {
        return static_cast<T*>(malloc_<M>(alignof(T), n * sizeof(T), m_memory_domain));
    }

    void deallocate(pointer p, size_type n)
    {
        free_<M>(p, alignof(T), n * sizeof(T), m_memory_domain);
    }

    //construct: use default std::allocator_traits implementation
    //destroy:   use default std::allocator_traits implementation
    //max_size:  use default std::allocator_traits implementation

    friend bool operator==(stateful_allocator const& a, stateful_allocator const& b) noexcept
    {
        return a.m_memory_domain == b.m_memory_domain;
    }

    friend bool operator!=(stateful_allocator const& a, stateful_allocator const& b) noexcept
    {
        return a.m_memory_domain != b.m_memory_domain;
    }
};

template<typename T, typename U, memory_type M>
void
swap(stateful_allocator<T, M>& a, stateful_allocator<U, M>& b) noexcept
{
    a.swap(b);
}

} // namespace hwmalloc
