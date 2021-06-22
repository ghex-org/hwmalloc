#include <gtest/gtest.h>
#include "./ucx_test_suite.hpp"

#include <hwmalloc/heap.hpp>
#include <hwmalloc/ucx/context.hpp>
#include <iostream>

TEST_F(ucx_test_suite, some_test)
{
    hwmalloc::ucx::context c(ucp_context);

    hwmalloc::heap<hwmalloc::ucx::context> h(&c);

    auto ptr = h.allocate(8, 0);

    std::cout << ptr.get() << std::endl;

    h.free(ptr);
}
