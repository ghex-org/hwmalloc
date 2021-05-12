#pragma once

namespace hwmalloc
{
enum class memory_type
{
    cpu,
    gpu
};

template<memory_type M>
struct memory_t
{
};

using cpu_t = memory_t<memory_type::cpu>;
using gpu_t = memory_t<memory_type::cpu>;
static constexpr cpu_t cpu;
static constexpr gpu_t gpu;

} // namespace hwmalloc
