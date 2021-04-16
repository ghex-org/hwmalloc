#pragma once

#include <hwmalloc/void_ptr.hpp>
#include <hwmalloc/memfct_ptr.hpp>

namespace hwmalloc
{
template<typename T, memory_type MemoryType>
struct hwptr
{
    hwvptr<MemoryType> m_ptr;
    T const& operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    T const* operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
    T&       operator*() noexcept { return *reinterpret_cast<T*>(m_ptr.get()); }
    T*       operator->() noexcept { return reinterpret_cast<T*>(m_ptr.get()); }

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

    //operator hwvptr<MemoryType>() const noexcept { return m_ptr; }
    operator bool() const noexcept { return (bool)m_ptr; }
};

template<typename T, memory_type MemoryType>
struct hwptr<const T, MemoryType>
{
    hwvptr<MemoryType> m_ptr;
    T const& operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    T const* operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
             operator bool() const noexcept { return (bool)m_ptr; }
};

} // namespace hwmalloc
