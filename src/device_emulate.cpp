#include <hwmalloc/device.hpp>
#include <cstdlib>
#include <cstring>

namespace hwmalloc
{
int
get_num_devices()
{
    return 1;
}

int
get_device_id()
{
    return 0;
}

void
set_device_id(int /*id*/)
{
}

void*
device_malloc(std::size_t size)
{
    return std::memset(std::malloc(size), 0, size);
}

void
device_free(void* ptr) noexcept
{
    std::free(ptr);
}

} // namespace hwmalloc

