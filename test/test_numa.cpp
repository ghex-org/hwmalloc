/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <string>
#include <gtest/gtest.h>

#include <hwmalloc/numa.hpp>
#include <iostream>

TEST(numa, discover)
{
    using namespace hwmalloc;

    EXPECT_TRUE(numa().is_initialized());

    auto print_nodes = [](std::string name, const auto& nodes) {
        std::cout << name << " (size = " << nodes.size() << "):   ";
        for (auto n : nodes) std::cout << n.first << " ";
        std::cout << std::endl;
    };

    print_nodes("host nodes  ", numa().host_nodes());
    print_nodes("local nodes ", numa().local_nodes());
    print_nodes("device nodes", numa().device_nodes());

    std::cout << "can allocate on 0: " << numa().can_allocate_on(0) << std::endl;
    std::cout << "can allocate on 1: " << numa().can_allocate_on(1) << std::endl;
    std::cout << "can allocate on 2: " << numa().can_allocate_on(2) << std::endl;
}

TEST(numa, allocate)
{
    using namespace hwmalloc;

    auto a = numa().allocate(16, 0);
    EXPECT_TRUE(a);                             // a should be a valid allocation
    EXPECT_EQ(a.node, 0);                       // a should be on node 0
    EXPECT_EQ(a.size, 16 * numa().page_size()); // a should be 16 pages
    // libnuma allocation should have succeeded
#ifdef HWMALLOC_NUMA_FOR_LOCAL
    EXPECT_TRUE(a.use_numa_free);
#else
    if (numa().local_node() == 0) EXPECT_FALSE(a.use_numa_free);
    else EXPECT_TRUE(a.use_numa_free);
#endif

    // use a's allocation
    new (a.ptr) int(42);
    std::cout << *static_cast<int*>(a.ptr) << std::endl;

    // free the memory
    numa().free(a);

    auto b = numa().allocate(1, 10000);         // try allocating on impossible node
    EXPECT_TRUE(b);                             // b should be a valid allocation
    EXPECT_EQ(b.node, numa().local_node());     // b should be on local node
    EXPECT_EQ(b.size, 1 * numa().page_size());  // b should be 1 page
    EXPECT_FALSE(b.use_numa_free);
    numa().free(b);

    auto c = numa().allocate_malloc(1);          // use normal malloc and lookup numa node
    EXPECT_TRUE(c);                              // c should be a valid allocation
    EXPECT_TRUE(numa().can_allocate_on(c.node)); // malloc should have chosen a local node
    EXPECT_EQ(c.size, 1 * numa().page_size());   // c should be 1 page
    EXPECT_FALSE(c.use_numa_free);               // use std::free for deallocation
    numa().free(c);

    auto d = numa().allocate(0); // try to allocate 0 memory
    EXPECT_FALSE(d);             // not a valid allocation
    numa().free(d);              // should succeed
}
