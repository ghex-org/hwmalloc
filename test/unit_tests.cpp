#include <gtest/gtest.h>
#include <hwmalloc/malloc.hpp>
#include <hwmalloc/new.hpp>
#include <cstdlib>
#include <iostream>

namespace hwmalloc
{
template<>
void*
raw_malloc<memory_type::cpu>(std::size_t s, int)
{
    return std::malloc(s);
}

template<>
void
raw_free<memory_type::cpu>(void* ptr, int)
{
    std::free(ptr);
}

} // namespace hwmalloc

TEST(example, no1)
{
    using namespace hwmalloc;
    //EXPECT_TRUE(true);
    //auto ptr = hwmalloc::malloc<hwmalloc::memory_type::cpu>(1);

    auto ptr = hw_new<int>(42);
    std::cout << *ptr << std::endl;
    hw_delete(ptr);
}
