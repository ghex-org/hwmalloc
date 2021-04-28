#include <hwmalloc/malloc.hpp>

namespace hwmalloc
{
namespace detail
{
class pool
{
  public:
    template<memory_type MemoryType>
    hw_vptr<MemoryType> malloc(std::size_t s) const
    {
        int memory_domain = 0;
        return hw_vptr<MemoryType>(raw_malloc<MemoryType>(s, memory_domain), memory_domain);
    }

    template<memory_type MemoryType>
    void free(hw_vptr<MemoryType> ptr, std::size_t size) const
    {
        raw_free<MemoryType>(ptr.m_data, size, ptr.m_memory_domain);
    }

    template<memory_type MemoryType>
    void free(hw_vptr<MemoryType> ptr, std::size_t alignment, std::size_t size) const
    {
        raw_free<MemoryType>(ptr.m_data, size, ptr.m_memory_domain);
    }

    template<memory_type MemoryType>
    void* malloc_(std::size_t s, int memory_domain) const
    {
        return raw_malloc<MemoryType>(s, memory_domain);
    }

    template<memory_type MemoryType>
    void free_(void* ptr, std::size_t size, int memory_domain) const
    {
        raw_free<MemoryType>(ptr, size, memory_domain);
    }

    template<memory_type MemoryType>
    void free_(void* ptr, std::size_t alignment, std::size_t size, int memory_domain) const
    {
        raw_free<MemoryType>(ptr, size, memory_domain);
    }
};

} // namespace detail

namespace
{
detail::pool&
get_pool() noexcept
{
    static detail::pool p_;
    return p_;
}
} // namespace

// cpu
template<>
hw_vptr<memory_type::cpu>
malloc(std::size_t s)
{
    return get_pool().template malloc<memory_type::cpu>(s);
}

template<>
hw_vptr<memory_type::cpu>
malloc(std::size_t alignment, std::size_t s)
{
    return get_pool().template malloc<memory_type::cpu>(s);
}

template<>
void*
malloc_<memory_type::cpu>(std::size_t size, int memory_domain)
{
    return get_pool().template malloc_<memory_type::cpu>(size, memory_domain);
}

template<>
void*
malloc_<memory_type::cpu>(std::size_t alignment, std::size_t size, int memory_domain)
{
    return get_pool().template malloc_<memory_type::cpu>(size, memory_domain);
}

template<>
void
free(hw_vptr<memory_type::cpu> p, std::size_t size)
{
    get_pool().free(p, size);
}

template<>
void
free(hw_vptr<memory_type::cpu> p, std::size_t alignment, std::size_t size)
{
    get_pool().free(p, alignment, size);
}

template<>
void
free_<memory_type::cpu>(void* ptr, std::size_t size, int memory_domain)
{
    get_pool().template free_<memory_type::cpu>(ptr, size, memory_domain);
}

template<>
void
free_<memory_type::cpu>(void* ptr, std::size_t alignment, std::size_t size, int memory_domain)
{
    get_pool().template free_<memory_type::cpu>(ptr, alignment, size, memory_domain);
}

#ifdef HWMALLOC_ENABLE_GPU
// gpu
template<>
hw_vptr<memory_type::gpu>
malloc(std::size_t s)
{
    return get_pool().template malloc<memory_type::gpu>(s);
}

template<>
hw_vptr<memory_type::gpu>
malloc(std::size_t alignment, std::size_t s)
{
    return get_pool().template malloc<memory_type::gpu>(s);
}

template<>
void
free(hw_vptr<memory_type::gpu> p, std::size_t size)
{
    get_pool().free(p, size);
}

template<>
void
free(hw_vptr<memory_type::gpu> p, std::size_t alignment, std::size_t size)
{
    get_pool().free(p, alignment, size);
}
#endif

} // namespace hwmalloc
