#include <hwmalloc/device.hpp>
//#include <hip_runtime_api.h>
#include <cstdlib>

namespace hwmalloc
{
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
    return std::malloc(size);
}

void
device_free(void* ptr)
{
    std::free(ptr);
}

} // namespace hwmalloc
