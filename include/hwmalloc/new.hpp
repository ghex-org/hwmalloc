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
    free((hw_vptr<memory_type::cpu>)ptr, alignof(T), sizeof(T));
}

template<typename T, typename... Args>
inline hw_ptr<T, memory_type::cpu>
hw_new_array(std::size_t n, Args&&... args)
{
    auto vptr = malloc<memory_type::cpu>(alignof(T), n * sizeof(T));
    T*   ptr = (T*)vptr.get();
    for (std::size_t i = 0; i < n; ++i) new (ptr + i) T(std::forward<Args>(args)...);
    return (hw_ptr<T, memory_type::cpu>)vptr;
}

template<typename T>
inline void
hw_delete_array(hw_ptr<T, memory_type::cpu> ptr, std::size_t n)
{
    for (std::size_t i = 0; i < n; ++i) (ptr + i)->~T();
    free((hw_vptr<memory_type::cpu>)ptr, alignof(T), n * sizeof(T));
}

} // namespace hwmalloc
