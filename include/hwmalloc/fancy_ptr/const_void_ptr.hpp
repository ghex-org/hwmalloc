/*
 * ghex-org
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/fancy_ptr/void_ptr.hpp>
#include <cstddef>
#include <type_traits>

namespace hwmalloc
{
template<typename Block>
class hw_void_ptr<Block, void const*>
{
  private:
    using this_type = hw_void_ptr<Block, void const*>;
    template<typename T, typename B>
    friend class hw_ptr;

  private:
    Block m_data;

  public:
    constexpr hw_void_ptr() noexcept {}
    constexpr hw_void_ptr(std::nullptr_t) noexcept {}
    constexpr hw_void_ptr(hw_void_ptr const&) noexcept = default;
    constexpr hw_void_ptr(hw_void_ptr<Block, void*> const& ptr) noexcept
    : m_data{ptr.m_data}
    {
    }
    hw_void_ptr& operator=(hw_void_ptr const&) noexcept = default;
    hw_void_ptr& operator=(std::nullptr_t) noexcept
    {
        m_data.m_ptr = nullptr;
        return *this;
    }
    template<typename T>
    constexpr hw_void_ptr& operator=(hw_ptr<T, Block> const& ptr) noexcept;

    constexpr friend bool operator==(hw_void_ptr a, hw_void_ptr b) noexcept
    {
        return (a.m_data.m_ptr == b.m_data.m_ptr);
    }
    constexpr friend bool operator!=(hw_void_ptr a, hw_void_ptr b) noexcept
    {
        return (a.m_data.m_ptr != b.m_data.m_ptr);
    }

    constexpr void const* get() const noexcept { return m_data.m_ptr; }

#if HWMALLOC_ENABLE_DEVICE
    constexpr void const* device_ptr() const noexcept { return m_data.m_device_ptr; }
#endif

    constexpr operator bool() const noexcept { return (bool)m_data.m_ptr; }

    template<typename T, typename = std::enable_if_t<std::is_const<T>::value>>
    constexpr explicit operator hw_ptr<T, Block>() const noexcept;
};

template<typename Block>
using hw_const_void_ptr = hw_void_ptr<Block, void const*>;

} // namespace hwmalloc
