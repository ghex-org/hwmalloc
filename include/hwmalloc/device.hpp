#pragma once

#include <cstddef>

namespace hwmalloc
{
int get_num_devices();

int get_device_id();

void set_device_id(int id);

void* device_malloc(std::size_t size);

void device_free(void* ptr) noexcept;

} // namespace hwmalloc
