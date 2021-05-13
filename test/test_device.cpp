#include <gtest/gtest.h>

#include <hwmalloc/device.hpp>
#include <iostream>

TEST(device, malloc)
{
    using namespace hwmalloc;

    auto ptr = device_malloc(128);

    device_free(ptr);
}

