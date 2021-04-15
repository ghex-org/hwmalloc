
#pragma once
#include <cstddef>

namespace hwmalloc
{
namespace detail
{
class pool;
}

enum class memory_type
{
    cpu,
    gpu
};

class hwvptr
{
    friend class detail::pool;

  private:
    void*       m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;
    memory_type m_type = memory_type::cpu;

  private:
    hwvptr() noexcept {}
    hwvptr(void* data, std::size_t size, int memory_domain, memory_type type) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    , m_type{type}
    {
    }

  public:
    hwvptr(hwvptr const&) noexcept = default;
    hwvptr(hwvptr&&) noexcept = default;
    hwvptr& operator=(hwvptr const&) noexcept = default;
    hwvptr& operator=(hwvptr&&) noexcept = default;
};

//template<typename T>
//class hwptr : public hwptr_base
//{
//};

hwvptr malloc(std::size_t);

void free(hwvptr const&);

} // namespace hwmalloc
