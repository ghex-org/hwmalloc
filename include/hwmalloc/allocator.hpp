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

#include <hwmalloc/fancy_ptr/ptr.hpp>
#include <hwmalloc/fancy_ptr/const_ptr.hpp>
#include <hwmalloc/allocator_base.hpp>

namespace hwmalloc
{

template<typename T, typename Heap>
class allocator : public allocator_base
{
  public:
    using this_type = allocator<T, Heap>;
    using block_type = typename Heap::block_type;
    using handle_type = typename Heap::handle_type;
    using value_type = T;
    using pointer = hw_ptr<T, block_type>;
    using const_pointer = hw_ptr<const T, block_type>;
    using void_pointer = hw_void_ptr<block_type>;
    using const_void_pointer = hw_const_void_ptr<block_type>;
    using difference_type = typename pointer::difference_type;
    using size_type = std::size_t;

    template<typename U>
    struct other_alloc
    {
        using other = allocator<U, Heap>;
    };
    template<typename U>
    using rebind = other_alloc<U>;

    Heap*       m_heap;
    std::size_t m_numa_node;

  public:
    allocator()
    {
        m_heap = Heap::get_instance().get();
        m_numa_node = 0;
    }

    // provide initializing constructor to help compiler
    allocator(Heap *heap, std::size_t numa_node)
        : m_heap{heap}
        , m_numa_node{numa_node}
    {}

    pointer allocate(size_type n) //, const_void_pointer = const_void_pointer())
    {
        return static_cast<pointer>(m_heap->allocate(n * sizeof(T), m_numa_node));
    }

    void deallocate(pointer const& p, size_type) { m_heap->free(static_cast<void_pointer>(p)); }

    allocator_base::anonymous_handle allocate_untyped(size_type n) override
    {
        // see segment.hpp to see what the contents of the fancy pointer are
        struct dummy {
            void *segment;
            void *addr;
        };
        auto p = allocate(n);
        dummy *temp = reinterpret_cast<dummy*>(&p);
        return {temp->segment, temp->addr, std::make_unique<handle_type>(p.handle())};
    }

    void deallocate_untyped(allocator_base::anonymous_handle &data) override
    {
        pointer *p = reinterpret_cast<pointer*>(&data);
        // I am not happy about ignoring the size param
        deallocate(*p, 0);
    }

    //construct: use default std::allocator_traits implementation
    //destroy:   use default std::allocator_traits implementation
    //max_size:  use default std::allocator_traits implementation
};

} // namespace hwmalloc
