# hwmalloc

This library provides a thread-safe [heap class](include/hwmalloc/heap.hpp) for allocating memory on given numa nodes and
devices (GPUs). The memory is requested from the OS/device-runtime in large segments which are kept alive.
After allocation of these segments, the memory is given to a user-supplied **Context** for registering with e.g.
a network transport layer. This is achieved through customization point objects (see [register.hpp](include/hwmalloc/register.hpp)
and [register_device.hpp](include/hwmalloc/register_device.hpp)).

The requested memory is returned in form of [fancy pointers](include/hwmalloc/fancy_ptr/void_ptr.hpp) which store
additional information about the memory segment from which they originated and provide
access to potential RMA keys that the network layer generates during registering.

If device (GPU) memory is requested, space will be allocated on both the device and the host
(effectively mirroring the memory). Both memory regions are passed to the **Context** for
registration. Note, that setting a numa node for device memory allocation is therefore still
necessary.

For integration with STL containers, there is a C++ [allocator class](include/hwmalloc/allocator.hpp). Note, that not all
containers support fancy pointers (*std::vector* is a container that will work).
