/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <gtest/gtest.h>

#include <hwmalloc/heap.hpp>
#include <iostream>

TEST(device, malloc)
{
    using namespace hwmalloc;

    auto ptr = device_malloc(128);

    device_free(ptr);
}

struct context
{
    int m = 42;
    context() { std::cout << "context constructor" << std::endl; }
    ~context() { std::cout << "context destructor" << std::endl; }

    struct region
    {
        struct handle_type
        {
            void* ptr;
        };

        void* ptr = nullptr;

        region(void* p) noexcept
        : ptr{p}
        {
        }

        region(region const&) = delete;

        region(region&& other) noexcept
        : ptr{std::exchange(other.ptr, nullptr)}
        {
        }

        ~region()
        {
            if (ptr) std::cout << "    region destructor" << std::endl;
        }

        handle_type get_handle(std::size_t offset, std::size_t /*size*/) const noexcept
        {
            return {(void*)((char*)ptr + offset)};
        }
    };
};

auto
register_memory(context&, void* ptr, std::size_t)
{
    return context::region{ptr};
}

TEST(heap, construction)
{
    using heap_t = hwmalloc::heap<context>;

    context c;

    heap_t h(&c);

    auto ptr = h.allocate(1, 0, 0);
    std::cout << ptr.get() << std::endl;
    std::cout << ptr.device_ptr() << std::endl;
    h.free(ptr);
}
