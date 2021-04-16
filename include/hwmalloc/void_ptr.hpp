#pragma once

#include <cstddef>
#include <type_traits>

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

template<typename T, memory_type MemoryType>
class hwptr;

template<memory_type MemoryType, typename VoidPtr = void*>
class hwvptr
{
  public:
    static constexpr memory_type type = MemoryType;

  private:
    using this_type = hwvptr<MemoryType, VoidPtr>;
    friend class detail::pool;
    friend class hwvptr<MemoryType, void const*>;

  private:
    VoidPtr     m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;

  private:
    constexpr hwvptr(void* data, std::size_t size, int memory_domain) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    {
    }

  public:
    constexpr hwvptr() noexcept {}
    constexpr hwvptr(hwvptr const&) noexcept = default;
    constexpr hwvptr(std::nullptr_t) noexcept {}
    hwvptr& operator=(hwvptr const&) noexcept = default;
    hwvptr& operator=(std::nullptr_t) noexcept
    {
        m_data = nullptr;
        m_size = 0u;
        m_memory_domain = -1;
        return *this;
    }
    template<typename T>
    constexpr hwvptr& operator=(hwptr<T, MemoryType> const& ptr) noexcept;

    constexpr friend bool operator==(hwvptr a, hwvptr b) noexcept { return (a.m_data == b.m_data); }
    constexpr friend bool operator!=(hwvptr a, hwvptr b) noexcept { return (a.m_data != b.m_data); }

    constexpr VoidPtr get() const noexcept { return m_data; }

    //constexpr hwvptr release() noexcept
    //{
    //    hwvptr tmp(*this);
    //    *this = hwvptr();
    //    return tmp;
    //}

    constexpr operator bool() const noexcept { return (bool)m_data; }

    template<typename T> //, typename = std::enable_if_t<!std::is_const<T>::value> >
    constexpr explicit operator hwptr<T, MemoryType>() const noexcept;
};

template<memory_type MemoryType>
class hwvptr<MemoryType, void const*>
{
  public:
    static constexpr memory_type type = MemoryType;

  private:
    using this_type = hwvptr<MemoryType, void const*>;
    friend class detail::pool;

  private:
    void const* m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;

  private:
    constexpr hwvptr(void const* data, std::size_t size, int memory_domain) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    {
    }

  public:
    constexpr hwvptr() noexcept {}
    constexpr hwvptr(std::nullptr_t) noexcept {}
    constexpr hwvptr(hwvptr const&) noexcept = default;
    constexpr hwvptr(hwvptr<MemoryType, void*> const& ptr) noexcept
    : m_data{ptr.m_data}
    , m_size{ptr.m_size}
    , m_memory_domain{ptr.m_memory_domain}
    {
    }
    hwvptr& operator=(hwvptr const&) noexcept = default;
    hwvptr& operator=(std::nullptr_t) noexcept
    {
        m_data = nullptr;
        m_size = 0u;
        m_memory_domain = -1;
        return *this;
    }
    template<typename T>
    constexpr hwvptr& operator=(hwptr<T, MemoryType> const& ptr) noexcept;

    constexpr friend bool operator==(hwvptr a, hwvptr b) noexcept { return (a.m_data == b.m_data); }
    constexpr friend bool operator!=(hwvptr a, hwvptr b) noexcept { return (a.m_data != b.m_data); }

    constexpr void const* get() const noexcept { return m_data; }

    constexpr operator bool() const noexcept { return (bool)m_data; }

    template<typename T, typename = std::enable_if_t<std::is_const<T>::value>>
    constexpr explicit operator hwptr<T, MemoryType>() const noexcept;
};

template<memory_type MemoryType>
using hwcvptr = hwvptr<MemoryType, void const*>;

} // namespace hwmalloc
