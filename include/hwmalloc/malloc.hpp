#pragma once

#include <hwmalloc/void_ptr.hpp>

namespace hwmalloc
{
// these functions must be implemented by user of library
// ------------------------------------------------------
template<memory_type MemoryType>
void* raw_malloc(std::size_t size, int memory_domain);

template<memory_type MemoryType>
void raw_free(void* ptr, std::size_t size, int memory_domain);

// allocate memory
// ---------------
template<memory_type MemoryType>
hw_vptr<MemoryType> malloc(std::size_t size);

template<memory_type MemoryType>
hw_vptr<MemoryType> malloc(std::size_t alignment, std::size_t size);

// free memory
// -----------
template<memory_type MemoryType>
void free(hw_vptr<MemoryType> ptr, std::size_t size);

template<memory_type MemoryType>
void free(hw_vptr<MemoryType> ptr, std::size_t alignment, std::size_t size);

} // namespace hwmalloc
