#include <hwmalloc/malloc.hpp>

namespace hwmalloc
{
namespace detail
{
class pool
{
  public:
    template<memory_type MemoryType>
    hwvptr<MemoryType> malloc(std::size_t s) const
    {
        hwvptr<MemoryType> x;
        x.m_data = raw_malloc<MemoryType>(s, 0);
        return x;
    }

    template<memory_type MemoryType>
    void free(hwvptr<MemoryType> ptr) const
    {
        raw_free<MemoryType>(ptr.m_data, ptr.m_memory_domain);
    }

    template<memory_type MemoryType>
    void free(hwvptr<MemoryType> ptr, std::size_t alignment) const
    {
        raw_free<MemoryType>(ptr.m_data, ptr.m_memory_domain);
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
hwvptr<memory_type::cpu>
malloc(std::size_t s)
{
    return get_pool().template malloc<memory_type::cpu>(s);
}

template<>
hwvptr<memory_type::cpu>
malloc(std::size_t alignment, std::size_t s)
{
    return get_pool().template malloc<memory_type::cpu>(s);
}

template<>
void
free(hwvptr<memory_type::cpu> p)
{
    get_pool().free(p);
}

template<>
void
free(hwvptr<memory_type::cpu> p, std::size_t alignment)
{
    get_pool().free(p, alignment);
}

#ifdef HWMALLOC_ENABLE_GPU
// gpu
template<>
hwvptr<memory_type::gpu>
malloc(std::size_t s)
{
    return get_pool().template malloc<memory_type::gpu>(s);
}

template<>
hwvptr<memory_type::gpu>
malloc(std::size_t alignment, std::size_t s)
{
    return get_pool().template malloc<memory_type::gpu>(s);
}

template<>
void
free(hwvptr<memory_type::gpu> p)
{
    get_pool().free(p);
}

template<>
void
free(hwvptr<memory_type::gpu> p, std::size_t alignment)
{
    get_pool().free(p, alignment);
}
#endif

} // namespace hwmalloc
