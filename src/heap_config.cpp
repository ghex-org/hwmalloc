/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <hwmalloc/config.hpp>
#include <hwmalloc/heap_config.hpp>

#ifdef HWMALLOC_ENABLE_LOGGING
#include <hwmalloc/log.hpp>
#include <string>
#endif

#include <cstdlib>
#include <stdexcept>

namespace hwmalloc
{
namespace detail
{
static std::size_t
get_env_size_t(const char* name, std::size_t default_value) noexcept
{
    const char* env_value = std::getenv(name);
    if (env_value)
    {
        try
        {
            return std::stoul(env_value);
        }
        catch (...)
        {
#ifdef HWMALLOC_ENABLE_LOGGING
            HWMALLOC_LOG("failed to parse configuration option", name, "=", env_value,
                ", using default =", default_value);
#endif
            return default_value;
        }
    }

    return default_value;
}
} // namespace detail

heap_config::heap_config(std::size_t tiny_limit, std::size_t small_limit, std::size_t large_limit,
    std::size_t tiny_segment_size, std::size_t small_segment_size, std::size_t large_segment_size)
: m_tiny_limit{detail::round_to_pow_of_2(tiny_limit)}
, m_small_limit{detail::round_to_pow_of_2(small_limit)}
, m_large_limit{detail::round_to_pow_of_2(large_limit)}
, m_tiny_segment_size{detail::round_to_pow_of_2(tiny_segment_size)}
, m_small_segment_size{detail::round_to_pow_of_2(small_segment_size)}
, m_large_segment_size{detail::round_to_pow_of_2(large_segment_size)}
{
    // Validate that tiny_limit < small_limit < large_limit
    if (!(m_tiny_limit < m_small_limit && m_small_limit < m_large_limit))
    {
        // TODO: Print actual values
        throw std::runtime_error(
            "Invalid heap size configuration: "
            "HWMALLOC_TINY_LIMIT < HWMALLOC_SMALL_LIMIT < HWMALLOC_LARGE_LIMIT must hold.");
    }

    // Validate that limits are at most segment sizes
    if (!(tiny_limit <= tiny_segment_size && small_limit <= small_segment_size &&
            large_limit <= large_segment_size))
    {
        // TODO: Print actual values
        throw std::runtime_error("Invalid heap size configuration: "
                                 "Limits must be at most segment sizes.");
    }
}

heap_config const&
get_default_heap_config()
{
    // TODO: Add comment for sizes
    static heap_config config{detail::get_env_size_t("HWMALLOC_TINY_LIMIT", (1u << 7)),
        detail::get_env_size_t("HWMALLOC_SMALL_LIMIT", (1u << 10)),
        detail::get_env_size_t("HWMALLOC_LARGE_LIMIT", (1u << 16)),
        detail::get_env_size_t("HWMALLOC_TINY_SEGMENT_SIZE", 0x04000),
        detail::get_env_size_t("HWMALLOC_SMALL_SEGMENT_SIZE", 0x08000),
        detail::get_env_size_t("HWMALLOC_LARGE_SEGMENT_SIZE", 0x10000)

    };

    return config;
}
} // namespace hwmalloc
