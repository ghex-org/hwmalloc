#include <gtest/gtest.h>

#include <hwmalloc/numa.hpp>
#include <iostream>

TEST(numa, discover)
{
    EXPECT_TRUE(hwmalloc::numa().is_initialized());

    std::cout << hwmalloc::numa().num_host_nodes() << std::endl;
    std::cout << hwmalloc::numa().can_allocate_on(0) << std::endl;
    std::cout << hwmalloc::numa().can_allocate_on(1) << std::endl;
    std::cout << hwmalloc::numa().can_allocate_on(2) << std::endl;

    auto a = hwmalloc::numa().allocate(16, 0);
    new (a.ptr) int(42);
    std::cout << *static_cast<int*>(a.ptr) << std::endl;
    hwmalloc::numa().free(a);
}
