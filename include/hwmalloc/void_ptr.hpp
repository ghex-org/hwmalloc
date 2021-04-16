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
class hw_ptr;

template<memory_type MemoryType, typename VoidPtr = void*>
class hw_vptr
{
  public:
    static constexpr memory_type type = MemoryType;

  private:
    using this_type = hw_vptr<MemoryType, VoidPtr>;
    friend class detail::pool;
    friend class hw_vptr<MemoryType, void const*>;

  private:
    VoidPtr     m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;

  private:
    constexpr hw_vptr(void* data, std::size_t size, int memory_domain) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    {
    }

  public:
    constexpr hw_vptr() noexcept {}
    constexpr hw_vptr(hw_vptr const&) noexcept = default;
    constexpr hw_vptr(std::nullptr_t) noexcept {}
    hw_vptr& operator=(hw_vptr const&) noexcept = default;
    hw_vptr& operator=(std::nullptr_t) noexcept
    {
        m_data = nullptr;
        m_size = 0u;
        m_memory_domain = -1;
        return *this;
    }
    template<typename T>
    constexpr hw_vptr& operator=(hw_ptr<T, MemoryType> const& ptr) noexcept;

    constexpr friend bool operator==(hw_vptr a, hw_vptr b) noexcept
    {
        return (a.m_data == b.m_data);
    }
    constexpr friend bool operator!=(hw_vptr a, hw_vptr b) noexcept
    {
        return (a.m_data != b.m_data);
    }

    constexpr VoidPtr get() const noexcept { return m_data; }
    void              set(VoidPtr ptr) noexcept { m_data = ptr; }

    //constexpr hw_vptr release() noexcept
    //{
    //    hw_vptr tmp(*this);
    //    *this = hw_vptr();
    //    return tmp;
    //}

    constexpr operator bool() const noexcept { return (bool)m_data; }

    template<typename T> //, typename = std::enable_if_t<!std::is_const<T>::value> >
    constexpr explicit operator hw_ptr<T, MemoryType>() const noexcept;
};

template<memory_type MemoryType>
class hw_vptr<MemoryType, void const*>
{
  public:
    static constexpr memory_type type = MemoryType;

  private:
    using this_type = hw_vptr<MemoryType, void const*>;
    friend class detail::pool;

  private:
    void const* m_data = nullptr;
    std::size_t m_size = 0u;
    int         m_memory_domain = -1;

  private:
    constexpr hw_vptr(void const* data, std::size_t size, int memory_domain) noexcept
    : m_data{data}
    , m_size{size}
    , m_memory_domain{memory_domain}
    {
    }

  public:
    constexpr hw_vptr() noexcept {}
    constexpr hw_vptr(std::nullptr_t) noexcept {}
    constexpr hw_vptr(hw_vptr const&) noexcept = default;
    constexpr hw_vptr(hw_vptr<MemoryType, void*> const& ptr) noexcept
    : m_data{ptr.m_data}
    , m_size{ptr.m_size}
    , m_memory_domain{ptr.m_memory_domain}
    {
    }
    hw_vptr& operator=(hw_vptr const&) noexcept = default;
    hw_vptr& operator=(std::nullptr_t) noexcept
    {
        m_data = nullptr;
        m_size = 0u;
        m_memory_domain = -1;
        return *this;
    }
    template<typename T>
    constexpr hw_vptr& operator=(hw_ptr<T, MemoryType> const& ptr) noexcept;

    constexpr friend bool operator==(hw_vptr a, hw_vptr b) noexcept
    {
        return (a.m_data == b.m_data);
    }
    constexpr friend bool operator!=(hw_vptr a, hw_vptr b) noexcept
    {
        return (a.m_data != b.m_data);
    }

    constexpr void const* get() const noexcept { return m_data; }
    void                  set(void const* ptr) noexcept { m_data = ptr; }

    constexpr operator bool() const noexcept { return (bool)m_data; }

    template<typename T, typename = std::enable_if_t<std::is_const<T>::value>>
    constexpr explicit operator hw_ptr<T, MemoryType>() const noexcept;
};

template<memory_type MemoryType>
using hw_cvptr = hw_vptr<MemoryType, void const*>;

} // namespace hwmalloc
