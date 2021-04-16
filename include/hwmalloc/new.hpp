#pragma once

#include <hwmalloc/malloc.hpp>

namespace hwmalloc
{
// new and delete (cpu only)
// -------------------------
template<typename T, typename... Args>
inline hwptr<T, memory_type::cpu>
hw_new(Args&&... args)
{
    auto ptr = malloc<T, memory_type::cpu>();
    new (ptr.m_ptr.get()) T(std::forward<Args>(args)...);
    return ptr;
}

template<typename T>
inline void
hw_delete(hwptr<T, memory_type::cpu>& ptr)
{
    ptr->~T();
    free<memory_type::cpu>(ptr.m_ptr);
}

} // namespace hwmalloc
