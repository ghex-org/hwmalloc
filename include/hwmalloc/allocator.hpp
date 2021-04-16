#pragma once

#include <hwmalloc/ptr.hpp>
#include <hwmalloc/malloc.hpp>

namespace hwmalloc {

template<typename T, memory_type M>
class allocator
{
    public:
    using this_type = allocator<T,M>;
    using value_type = T;
    using pointer = hw_ptr<T,M>;
    using const_pointer = hw_ptr<const T, M>;
    using void_pointer = hw_vptr<M>;
    using const_void_pointer = hw_cvptr<M>;
    using difference_type = typename pointer::difference_type;
    using size_type = std::size_t;

    template<typename U>
    struct other_alloc { using other = allocator<U,M>; };
    template<typename U>
    using rebind = other_alloc<U>;

    pointer allocate(size_type n)//, const_void_pointer = const_void_pointer())
    {
        return (pointer)hwmalloc::malloc<M>(alignof(T),n*sizeof(T));
    }

    void deallocate(pointer p, size_type /*n*/)
    {
        hwmalloc::free((void_pointer)p, alignof(T));
    }

    //construct: use default std::allocator_traits implementation
    //destroy:   use default std::allocator_traits implementation
    //max_size:  use default std::allocator_traits implementation

};


} // namespace hwmalloc
