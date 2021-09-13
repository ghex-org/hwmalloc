/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/config.hpp>
#include <cstddef>
#include <type_traits>

namespace hwmalloc
{
template<typename Context>
class heap;
template<typename T, typename Block>
class hw_ptr;

template<typename Block, typename VoidPtr = void*>
class hw_void_ptr
{
  private:
    using this_type = hw_void_ptr<Block, VoidPtr>;
    template<typename Context>
    friend class heap;
    friend class hw_void_ptr<Block, void const*>;
    template<typename T, typename B>
    friend class hw_ptr;

  public:
    using handle_type = typename Block::handle_type;
#if HWMALLOC_ENABLE_DEVICE
    using device_handle_type = typename Block::device_handle_type;
#endif

  private:
    Block m_data;

  private:
    constexpr hw_void_ptr(Block const& b) noexcept
    : m_data{b}
    {
    }

  public:
    constexpr hw_void_ptr() noexcept {}
    constexpr hw_void_ptr(hw_void_ptr const&) noexcept = default;
    constexpr hw_void_ptr(std::nullptr_t) noexcept {}
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

    constexpr VoidPtr get() const noexcept { return m_data.m_ptr; }

    auto        handle() const noexcept { return m_data.m_handle; }
    const auto& handle_ref() const noexcept { return m_data.m_handle; }
    auto&       handle_ref() noexcept { return m_data.m_handle; }

#if HWMALLOC_ENABLE_DEVICE
    constexpr VoidPtr device_ptr() const noexcept { return m_data.m_device_ptr; }

    auto device_handle() const noexcept { return m_data.m_device_handle; }

    int device_id() const noexcept { return m_data.m_device_id; }
#endif

    bool on_device() const noexcept { return m_data.on_device(); }

    constexpr operator bool() const noexcept { return (bool)m_data.m_ptr; }

    template<typename T>
    constexpr explicit operator hw_ptr<T, Block>() const noexcept;

    void release() { m_data.release(); }
};

} // namespace hwmalloc
