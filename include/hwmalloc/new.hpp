#pragma once

#include <hwmalloc/malloc.hpp>
#include <iostream>

namespace hwmalloc
{
// new and delete (cpu only)
// -------------------------
template<typename T, typename... Args>
inline hwptr<T, memory_type::cpu>
hw_new(Args&&... args)
{
    auto vptr = malloc<memory_type::cpu>(alignof(T),sizeof(T));
    new (vptr.get()) T(std::forward<Args>(args)...);
    return (hwptr<T,memory_type::cpu>)vptr;
}

template<typename T>
inline void
hw_delete(hwptr<T, memory_type::cpu> ptr)
{
    ptr->~T();
    free<memory_type::cpu>((hwvptr<memory_type::cpu>)ptr, alignof(T));
}

} // namespace hwmalloc
