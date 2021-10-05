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

#include <hwmalloc/fancy_ptr/ptr.hpp>
#include <memory>

namespace hwmalloc
{
// scalar version
template<typename T, typename Block>
struct heap_delete
{
    using pointer = hw_ptr<T, Block>;
    void operator()(pointer ptr) const noexcept
    {
        ptr->~T();
        ptr.release();
    }
};

// array version
template<typename T, typename Block>
struct heap_delete<T[], Block>
{
    using pointer = hw_ptr<T, Block>;
    std::size_t size;
    void        operator()(pointer ptr) const noexcept
    {
        for (std::size_t i = 0; i < size; ++i) ptr[i].~T();
        ptr.release();
    }
};

template<typename T, typename Block>
using unique_ptr = std::unique_ptr<T, heap_delete<T, Block>>;

} // namespace hwmalloc
