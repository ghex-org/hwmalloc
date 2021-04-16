#pragma once

#include <hwmalloc/void_ptr.hpp>
#include <hwmalloc/memfct_ptr.hpp>

namespace hwmalloc
{
template<typename T, memory_type MemoryType>
class hwptr
{
  private:
    friend class hwvptr<MemoryType, void*>;
    using void_ptr_t = hwvptr<MemoryType, void*>;
    using const_void_ptr_t = hwvptr<MemoryType, void const*>;
    void_ptr_t m_ptr;

  public:
    constexpr hwptr() noexcept = default;
    constexpr hwptr(hwptr const&) noexcept = default;
    constexpr hwptr(std::nullptr_t) noexcept
    : m_ptr{nullptr}
    {
    }
    hwptr& operator=(hwptr const&) noexcept = default;
    hwptr& operator=(std::nullptr_t) noexcept
    {
        m_ptr = nullptr;
        return *this;
    }

    constexpr friend bool operator==(hwptr a, hwptr b) noexcept { return (a.m_ptr == b.m_ptr); }
    constexpr friend bool operator!=(hwptr a, hwptr b) noexcept { return (a.m_ptr != b.m_ptr); }

  public:
    //T const& operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    //T const* operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
    T& operator*() const noexcept { return *reinterpret_cast<T*>(m_ptr.get()); }
    T* operator->() const noexcept { return reinterpret_cast<T*>(m_ptr.get()); }
    T* get() const noexcept { return reinterpret_cast<T*>(m_ptr.get()); }

    //template<typename R, typename... Args>
    //const pmfc<R (T::*)(Args...)> operator->*(R (T::*pmf)(Args...)) const noexcept
    //{
    //    return {m_ptr.get(), pmf};
    //}
    //template<typename R, typename... Args>
    //const pmfc<R (T::*)(Args...) const> operator->*(R (T::*pmf)(Args...) const) const noexcept
    //{
    //    return {m_ptr.get(), pmf};
    //}

    //template<typename U>
    //U& operator->*(U T::* pm) const noexcept
    //{
    //    return reinterpret_cast<T*>(m_ptr.get())->*pm;
    //}

    constexpr explicit operator void_ptr_t() const noexcept { return m_ptr; }
    constexpr explicit operator const_void_ptr_t() const noexcept { return m_ptr; }
    constexpr          operator bool() const noexcept { return (bool)m_ptr; }
};

template<memory_type MemoryType, typename VoidPtr>
template<typename T>
constexpr hwvptr<MemoryType, VoidPtr>&
hwvptr<MemoryType, VoidPtr>::operator=(hwptr<T, MemoryType> const& ptr) noexcept
{
    return *this = (this_type)ptr;
}

template<memory_type MemoryType, class VoidPtr>
template<class T>
constexpr hwvptr<MemoryType, VoidPtr>::operator hwptr<T, MemoryType>() const noexcept
{
    auto p = hwptr<T, MemoryType>();
    p.m_ptr = *this;
    return p;
}

template<typename T, memory_type MemoryType>
class hwptr<const T, MemoryType>
{
  private:
    friend class hwvptr<MemoryType, void*>;
    friend class hwvptr<MemoryType, void const*>;
    using void_ptr_t = hwvptr<MemoryType, void*>;
    using const_void_ptr_t = hwvptr<MemoryType, void const*>;
    const_void_ptr_t m_ptr;

  public:
    constexpr hwptr() noexcept = default;
    constexpr hwptr(hwptr const&) noexcept = default;
    constexpr hwptr(hwptr<T, MemoryType> const& ptr) noexcept
    : m_ptr((void_ptr_t)ptr)
    {
    }
    constexpr hwptr(std::nullptr_t) noexcept
    : m_ptr{nullptr}
    {
    }
    hwptr& operator=(hwptr const&) noexcept = default;
    hwptr& operator=(std::nullptr_t) noexcept
    {
        m_ptr = nullptr;
        return *this;
    }

    constexpr friend bool operator==(hwptr a, hwptr b) noexcept { return (a.m_ptr == b.m_ptr); }
    constexpr friend bool operator!=(hwptr a, hwptr b) noexcept { return (a.m_ptr != b.m_ptr); }

  public:
    T const& operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    T const* operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
    T const* get() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }

    constexpr explicit operator const_void_ptr_t() const noexcept { return m_ptr; }
    constexpr          operator bool() const noexcept { return (bool)m_ptr; }
};

template<memory_type MemoryType>
template<typename T>
constexpr hwvptr<MemoryType, void const*>&
hwvptr<MemoryType, void const*>::operator=(hwptr<T, MemoryType> const& ptr) noexcept
{
    return *this = (this_type)ptr;
}

template<memory_type MemoryType>
template<class T, typename>
constexpr hwvptr<MemoryType, void const*>::operator hwptr<T, MemoryType>() const noexcept
{
    auto p = hwptr<T, MemoryType>();
    p.m_ptr = *this;
    return p;
}

} // namespace hwmalloc
