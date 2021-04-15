#include <gtest/gtest.h>
#include <hwmalloc/hwmalloc.hpp>

TEST(example, no1)
{
    EXPECT_TRUE(true);
    auto ptr = malloc(1);
}
