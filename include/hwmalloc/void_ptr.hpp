#pragma once

#include <cstddef>

namespace hwmalloc
{
namespace detail
{
class pool;
} // namespace detail

enum class memory_type
{
    cpu,
    gpu
};

template<memory_type MemoryType>
class hwvptr
{
  public:
    static constexpr memory_type type = MemoryType;

  private:
    friend class detail::pool;

  private:
    void*       m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;

  private:
    hwvptr() noexcept {}
    hwvptr(void* data, std::size_t size, int memory_domain) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    {
    }

  public:
    hwvptr(hwvptr const&) noexcept = default;
    hwvptr& operator=(hwvptr const&) noexcept = default;

    void* get() const noexcept { return m_data; }

    hwvptr release() noexcept
    {
        hwvptr tmp(*this);
        *this = hwvptr();
        return tmp;
    }

    operator bool() const noexcept { return (bool)m_data; }
};

} // namespace hwmalloc
