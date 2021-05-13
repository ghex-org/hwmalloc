#pragma once

#include <hwmalloc/config.hpp>
#include <hwmalloc/register.hpp>
#if HWMALLOC_ENABLE_DEVICE
#include <hwmalloc/register_device.hpp>
#pragma message "Device is enabled"
#endif

namespace hwmalloc
{
namespace detail
{
template<typename Context>
struct region_traits
{
    using region_type = decltype(hwmalloc::register_memory(*((Context*)0), nullptr, 0u));

    static_assert(!std::is_copy_constructible<region_type>::value, "region is copy constructible");
    static_assert(
        std::is_move_constructible<region_type>::value, "region is not move constructible");

    using handle_type = typename region_type::handle_type;

    static_assert(
        std::is_default_constructible<handle_type>::value, "handle is not default constructible");
    static_assert(
        std::is_copy_constructible<handle_type>::value, "handle is not copy constructible");
    static_assert(std::is_copy_assignable<handle_type>::value, "handle is not copy assignable");

#if HWMALLOC_ENABLE_DEVICE
    using device_region_type =
        decltype(hwmalloc::register_device_memory(*((Context*)0), nullptr, 0u));

    static_assert(!std::is_copy_constructible<device_region_type>::value,
        "device_region is copy constructible");
    static_assert(std::is_move_constructible<device_region_type>::value,
        "device_region is not move constructible");

    using device_handle_type = typename device_region_type::handle_type;

    static_assert(std::is_default_constructible<device_handle_type>::value,
        "device_handle is not default constructible");
    static_assert(std::is_copy_constructible<device_handle_type>::value,
        "device_handle is not copy constructible");
    static_assert(
        std::is_copy_assignable<device_handle_type>::value, "device_handle is not copy assignable");
#endif
};

} // namespace detail
} // namespace hwmalloc
