#pragma once

#include <hwmalloc/ptr.hpp>

namespace hwmalloc
{
// these functions must be implemented by user of library
// ------------------------------------------------------
template<memory_type MemoryType>
void* raw_malloc(std::size_t, int);

template<memory_type MemoryType>
void raw_free(void*, int);

// allocate memory
// ---------------
template<memory_type MemoryType>
hwvptr<MemoryType> malloc(std::size_t size);

template<memory_type MemoryType>
hwvptr<MemoryType> malloc(std::size_t alignment, std::size_t size);

template<typename T, memory_type MemoryType>
inline hwptr<T, MemoryType>
malloc()
{
    return {malloc<MemoryType>(alignof(T), sizeof(T))};
}

// free memory
// -----------
template<memory_type MemoryType>
void free(hwvptr<MemoryType>& ptr);

template<memory_type MemoryType>
void free(hwvptr<MemoryType>& ptr, std::size_t alignment);

} // namespace hwmalloc
