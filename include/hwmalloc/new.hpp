#pragma once

#include <hwmalloc/ptr.hpp>
#include <hwmalloc/malloc.hpp>

namespace hwmalloc
{
// new and delete (cpu only)
// -------------------------
template<typename T, typename... Args>
inline hw_ptr<T, memory_type::cpu>
hw_new(Args&&... args)
{
    auto vptr = malloc<memory_type::cpu>(alignof(T), sizeof(T));
    new (vptr.get()) T(std::forward<Args>(args)...);
    return (hw_ptr<T, memory_type::cpu>)vptr;
}

template<typename T>
inline void
hw_delete(hw_ptr<T, memory_type::cpu> ptr)
{
    ptr->~T();
    free/*<memory_type::cpu>*/((hw_vptr<memory_type::cpu>)ptr, alignof(T));
}

} // namespace hwmalloc
