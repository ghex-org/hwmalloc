#include <gtest/gtest.h>
#include <hwmalloc/malloc.hpp>
#include <hwmalloc/new.hpp>
#include <hwmalloc/allocator.hpp>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <vector>

// concepts for VoidPointer
// --------
// - NullablePointer
//   - EqualityComparable
//   - DefaultConstructible
//   - CopyConstructible
//   - CopyAssignable
//   - Destructible

// concepts for Pointer
// - NullablePointer
//   - EqualityComparable
//   - DefaultConstructible
//   - CopyConstructible
//   - CopyAssignable
//   - Destructible
// - LegacyRandomAccessIterator
//   - LegacyBidirectionalIterator
//     - LegacyForwardIterator
//       - LegacyInputIterator
//         - LegacyIterator
//           - Swappable
//           - iterator_traits::reference
//           - iterator_traits::pointer
//           - iterator_traits::difference_type
//           - iterator_traits::value_type
//           - iterator_traits::iterator_category
//           - *r
//           - ++r
//         - i != j
//         - *i
//         - i->m
//       -
//     - iterator_traits::reference
//     - --it
//     - it--
//     - *a--
//   -
// - convertible to VoidPointer

template<typename Ptr>
void
check_nullable_pointer()
{
    using namespace hwmalloc;

    static_assert(std::is_default_constructible<Ptr>::value, "default constructible");
    static_assert(std::is_nothrow_default_constructible<Ptr>::value, "default constructible");
    //static_assert(std::is_trivially_default_constructible<Ptr>::value, "default constructible");
    static_assert(std::is_copy_constructible<Ptr>::value, "copy constructible");
    static_assert(std::is_nothrow_copy_constructible<Ptr>::value, "copy constructible");
    static_assert(std::is_trivially_copy_constructible<Ptr>::value, "copy constructible");
    static_assert(std::is_copy_assignable<Ptr>::value, "copy assignable");
    static_assert(std::is_nothrow_copy_assignable<Ptr>::value, "copy assignable");
    static_assert(std::is_trivially_copy_assignable<Ptr>::value, "copy assignable");
    static_assert(std::is_destructible<Ptr>::value, "destructible");
    static_assert(std::is_nothrow_destructible<Ptr>::value, "destructible");
    static_assert(std::is_trivially_destructible<Ptr>::value, "destructible");

    // additional expressions
    {
        Ptr p(nullptr);
        EXPECT_EQ(p, nullptr);
    }
    {
        Ptr p = nullptr;
        EXPECT_EQ(p, nullptr);
    }
    {
        EXPECT_EQ(Ptr(nullptr), nullptr);
    }
    {
        Ptr p;
        static_assert(std::is_same<decltype(p = nullptr), Ptr&>::value, "assignment return type");
        p = nullptr;
        EXPECT_EQ(p, nullptr);
    }
    {
        Ptr p, q;
        EXPECT_EQ(p == q, !(p != q));
        EXPECT_EQ(!(p == q), p != q);
        EXPECT_EQ(p == q, true);
        EXPECT_EQ(p != q, false);
    }
    {
        Ptr p;
        EXPECT_EQ(p == nullptr, !(p != nullptr));
        EXPECT_EQ(!(p == nullptr), p != nullptr);
        EXPECT_EQ(p == nullptr, true);
        EXPECT_EQ(p != nullptr, false);
    }
}

TEST(void_ptr, NullablePointer)
{
    using namespace hwmalloc;
    constexpr auto M = memory_type::cpu;
    using void_ptr_t = hw_vptr<M>;
    using const_void_ptr_t = hw_cvptr<M>;

    check_nullable_pointer<void_ptr_t>();
    check_nullable_pointer<const_void_ptr_t>();

    // check conversions to const void ptr
    {
        void_ptr_t       p;
        const_void_ptr_t cp(p);
    }
    {
        void_ptr_t       p;
        const_void_ptr_t cp;
        cp = p;
    }
    {
        void_ptr_t       p;
        const_void_ptr_t cp;
        EXPECT_EQ(p == cp, true);
        EXPECT_EQ(p == cp, cp == p);
        EXPECT_EQ(p != cp, false);
        EXPECT_EQ(p != cp, cp != p);
    }
}

TEST(T_ptr, NullablePointer)
{
    using namespace hwmalloc;
    constexpr auto M = memory_type::cpu;
    using T_ptr_t = hw_ptr<int, M>;
    using const_T_ptr_t = hw_ptr<const int, M>;

    check_nullable_pointer<T_ptr_t>();
    check_nullable_pointer<const_T_ptr_t>();
}

TEST(T_ptr, conversions)
{
    using namespace hwmalloc;
    constexpr auto M = memory_type::cpu;
    using void_ptr_t = hw_vptr<M>;
    using const_void_ptr_t = hw_cvptr<M>;
    using T_ptr_t = hw_ptr<int, M>;
    using const_T_ptr_t = hw_ptr<const int, M>;

    void_ptr_t       vptr;
    const_void_ptr_t cvptr;
    T_ptr_t          Tptr;
    const_T_ptr_t    cTptr;

    // converting from void* to void const *
    const_void_ptr_t cvptr2(vptr);
    cvptr = vptr;

    // converting from T* to T const *
    const_T_ptr_t cTptr2(Tptr);
    cTptr = Tptr;

    // converting from T* to void *
    void_ptr_t vptr2(Tptr);
    vptr = Tptr;

    // converting from T const * to void -> error
    //void_ptr_t vptr3(cTptr);
    //vptr = cTptr;

    // converting from T const* to void const *
    const_void_ptr_t cvptr3(cTptr);
    cvptr = cTptr;

    // converting from T * to void const *
    const_void_ptr_t cvptr4(Tptr);
    cvptr = Tptr;

    // explicit conversion from void * to T *
    Tptr = (T_ptr_t)vptr;

    // explicit conversion from void * to T const *
    cTptr = (const_T_ptr_t)vptr;

    // explicit conversion from void const * to T * -> error
    //Tptr = (T_ptr_t)cvptr;

    // explicit conversion from void const * to T const *
    cTptr = (const_T_ptr_t)cvptr;
}

namespace hwmalloc
{
template<>
void*
raw_malloc<memory_type::cpu>(std::size_t s, int)
{
    std::cout << "allocating " << s << " bytes" << std::endl;
    return std::malloc(s);
}

template<>
void
raw_free<memory_type::cpu>(void* ptr, int)
{
    std::free(ptr);
}

} // namespace hwmalloc

TEST(new_delete, no1)
{
    using namespace hwmalloc;
    auto ptr = hw_new<int>(42);
    //std::cout << *ptr << std::endl;
    EXPECT_EQ(42, *ptr);
    hw_delete(ptr);
}

TEST(allocator, vector)
{
    using namespace hwmalloc;

    std::vector<int, allocator<int, memory_type::cpu>> vec;

    vec.reserve(10);
    vec.resize(20);

    std::cout << vec[15] << std::endl;

    for (auto x : vec)
        std::cout << x << " ";
    std::cout << std::endl;

}
