/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <memory>
#include <hwmalloc/fancy_ptr/ptr.hpp>

namespace hwmalloc
{
template<typename T, typename Block>
class hw_ptr<const T, Block>
{
  private:
    friend class hw_void_ptr<Block, void*>;
    friend class hw_void_ptr<Block, void const*>;
    using this_type = hw_ptr<const T, Block>;
    using void_ptr_t = hw_void_ptr<Block, void*>;
    using const_void_ptr_t = hw_void_ptr<Block, void const*>;

  private:
    const_void_ptr_t m_ptr;

  public: // iteator typedefs
    using value_type = T const;
    using difference_type = std::ptrdiff_t;
    using pointer = T const*;
    using reference = T const&;
    using iterator_category = std::random_access_iterator_tag;

  public:
    constexpr hw_ptr() noexcept = default;
    constexpr hw_ptr(hw_ptr const&) noexcept = default;
    constexpr hw_ptr(hw_ptr<T, Block> const& ptr) noexcept
    : m_ptr((void_ptr_t)ptr)
    {
    }
    constexpr hw_ptr(std::nullptr_t) noexcept
    : m_ptr{nullptr}
    {
    }
    hw_ptr& operator=(hw_ptr const&) noexcept = default;
    hw_ptr& operator=(std::nullptr_t) noexcept
    {
        m_ptr = nullptr;
        return *this;
    }

    constexpr friend bool operator==(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr == b.m_ptr); }
    constexpr friend bool operator!=(hw_ptr a, hw_ptr b) noexcept { return (a.m_ptr != b.m_ptr); }

  public:
    reference operator*() const noexcept { return *reinterpret_cast<T const*>(m_ptr.get()); }
    pointer   operator->() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
    pointer   get() const noexcept { return reinterpret_cast<T const*>(m_ptr.get()); }
#if HWMALLOC_ENABLE_DEVICE
    pointer device_ptr() const noexcept { return reinterpret_cast<T const*>(m_ptr.device_ptr()); }
#endif

    // support for pointer to const member function
    template<typename R, typename U, typename... Args>
    typename std::enable_if<std::is_same<U, T>::value && std::is_class<U>::value,
        const pmfc<R (U::*)(Args...) const>>::type
    operator->*(R (U::*pmf)(Args...) const) const noexcept
    {
        return {get(), pmf};
    }

    // support for pointer const to member
    template<typename M, typename U>
    typename std::enable_if<std::is_same<U, T>::value && std::is_class<U>::value, M const&>::type
    operator->*(const M U::*pm) const noexcept
    {
        return get()->*pm;
    }

    constexpr explicit operator const_void_ptr_t() const noexcept { return m_ptr; }
    constexpr          operator bool() const noexcept { return (bool)m_ptr; }

  public: // iterator functions
    this_type& operator++() noexcept
    {
        m_ptr.m_data.m_ptr = get() + 1;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() + 1;
#endif
        return *this;
    }

    this_type operator++(int) noexcept
    {
        auto tmp = *this;
        m_ptr.m_data.m_ptr = get() + 1;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() + 1;
#endif
        return tmp;
    }

    this_type& operator+=(std::ptrdiff_t n) noexcept
    {
        m_ptr.m_data.m_ptr = get() + n;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() + n;
#endif
        return *this;
    }

    friend this_type operator+(this_type a, std::size_t n) noexcept { return (a += n); }

    this_type& operator--() noexcept
    {
        m_ptr.m_data.m_ptr = get() - 1;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() - 1;
#endif
        return *this;
    }

    this_type operator--(int) noexcept
    {
        auto tmp = *this;
        m_ptr.m_data.m_ptr = get() - 1;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() - 1;
#endif
        return tmp;
    }

    this_type& operator-=(std::ptrdiff_t n) noexcept
    {
        m_ptr.m_data.m_ptr = get() - n;
#if HWMALLOC_ENABLE_DEVICE
        if (m_ptr.device_ptr()) m_ptr.m_data.m_device_ptr = device_ptr() - n;
#endif
        return *this;
    }

    friend this_type operator-(this_type a, std::size_t n) noexcept { return (a -= n); }

    friend difference_type operator-(this_type const& a, this_type const& b) noexcept
    {
        return (a.get() - b.get());
    }
};

template<typename Block>
template<typename T>
constexpr hw_void_ptr<Block, void const*>&
hw_void_ptr<Block, void const*>::operator=(hw_ptr<T, Block> const& ptr) noexcept
{
    return *this = (this_type)ptr;
}

template<typename Block>
template<class T, typename>
constexpr hw_void_ptr<Block, void const*>::operator hw_ptr<T, Block>() const noexcept
{
    auto p = hw_ptr<T, Block>();
    p.m_ptr = *this;
    return p;
}

namespace detail {

template<typename A, typename B>
struct _rebind;

template<typename T, typename Block>
struct _rebind<hw_ptr<T, Block>, void>
{
    using type = hw_void_ptr<Block, void*>;
};

template<typename T, typename Block>
struct _rebind<hw_ptr<T, Block>, const void>
{
    using type = hw_void_ptr<Block, const void*>;
};

template<typename T, typename Block, typename U>
struct _rebind<hw_ptr<T, Block>, U>
{
    using type = hw_ptr<U, Block>;
};


template<typename Block>
struct _rebind<hw_void_ptr<Block, void*>, void>
{
    using type = hw_void_ptr<Block, void*>;
};

template<typename Block>
struct _rebind<hw_void_ptr<Block, void*>, const void>
{
    using type = hw_void_ptr<Block, const void*>;
};

template<typename Block, typename U>
struct _rebind<hw_void_ptr<Block, void*>, U>
{
    using type = hw_ptr<U, Block>;
};


template<typename Block>
struct _rebind<hw_void_ptr<Block, const void*>, void>
{
    using type = hw_void_ptr<Block, void*>;
};

template<typename Block>
struct _rebind<hw_void_ptr<Block, const void*>, const void>
{
    using type = hw_void_ptr<Block, const void*>;
};

template<typename Block, typename U>
struct _rebind<hw_void_ptr<Block, const void*>, U>
{
    using type = hw_ptr<U, Block>;
};


template<typename A, typename B>
using _rebind_t = typename _rebind<A, B>::type;

} // namespace detail

} // namespace hwmalloc

namespace std {

template<typename Block>
struct pointer_traits<hwmalloc::hw_void_ptr<Block, void*>> {
    using pointer = hwmalloc::hw_void_ptr<Block, void*>;
    using element_type = void;
    using difference_type = std::ptrdiff_t;

    template<class U>
    using rebind = typename ::hwmalloc::detail::template _rebind_t<pointer,U>;

    static element_type* to_address(pointer p) noexcept { return p.get(); }
};

template<typename Block>
struct pointer_traits<hwmalloc::hw_void_ptr<Block, const void*>> {
    using pointer = hwmalloc::hw_void_ptr<Block, const void*>;
    using element_type = const void;
    using difference_type = std::ptrdiff_t;

    template<class U>
    using rebind = typename ::hwmalloc::detail::template _rebind_t<pointer,U>;

    static element_type* to_address(pointer p) noexcept { return p.get(); }
};

template<typename T, typename Block>
struct pointer_traits<hwmalloc::hw_ptr<T, Block>> {
    using pointer = hwmalloc::hw_ptr<T, Block>;
    using element_type = T;
    using difference_type = std::ptrdiff_t;

    template<class U>
    using rebind = typename ::hwmalloc::detail::template _rebind_t<pointer,U>;

    static element_type* to_address(pointer p) noexcept { return p.get(); }
};

} // namespace std
