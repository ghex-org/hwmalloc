#include <gtest/gtest.h>
#include <hwmalloc/hwmalloc.hpp>

namespace hwmalloc {

template<>
void* raw_malloc<memory_type::cpu>(std::size_t, int)
{
    return nullptr;
}

}

TEST(example, no1)
{
    EXPECT_TRUE(true);
    auto ptr = malloc(1);
}
