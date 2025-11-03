/*
 * ghex-org
 *
 * Copyright (c) 2014-2025, ETH Zurich
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
#include <sstream>
#include <stdexcept>

namespace hwmalloc
{
namespace detail
{
template<typename T>
T
parse_value(const char* env_value)
{
    // This currently only supports the types needed within this file. If this
    // should be used elsewhere this can be generalized to something that can be
    // extended e.g. with template specializations instead a hardcoded list with
    // if-constexpr.
    if constexpr (std::is_same_v<T, std::size_t>) { return std::stoul(env_value); }
    else if constexpr (std::is_same_v<T, bool>) { return std::stoul(env_value) != 0; }
    else { static_assert(sizeof(T) == 0, "Unsupported type for parse_value"); }
}

template<typename T>
bool
get_env(const char* name, T default_value) noexcept
{
    const char* env_value = std::getenv(name);
    if (env_value)
    {
        try
        {
            return parse_value<T>(env_value);
        }
        catch (...)
        {
#ifdef HWMALLOC_ENABLE_LOGGING
            HWMALLOC_LOG("failed to parse boolean configuration option", name, "=", env_value,
                "(expected 0 or 1), using default =", default_value);
#endif
            return default_value;
        }
    }

    return default_value;
}
} // namespace detail

heap_config::heap_config(bool never_free, std::size_t num_reserve_segments, std::size_t tiny_limit,
    std::size_t small_limit, std::size_t large_limit, std::size_t tiny_segment_size,
    std::size_t small_segment_size, std::size_t large_segment_size)
: m_never_free{never_free}
, m_num_reserve_segments{num_reserve_segments}
, m_tiny_limit{detail::round_to_pow_of_2(tiny_limit)}
, m_small_limit{detail::round_to_pow_of_2(small_limit)}
, m_large_limit{detail::round_to_pow_of_2(large_limit)}
, m_tiny_segment_size{detail::round_to_pow_of_2(tiny_segment_size)}
, m_small_segment_size{detail::round_to_pow_of_2(small_segment_size)}
, m_large_segment_size{detail::round_to_pow_of_2(large_segment_size)}
{
    // Validate that tiny_limit < small_limit < large_limit
    if (!(m_tiny_limit < m_small_limit && m_small_limit < m_large_limit))
    {
        std::ostringstream os;
        os << "Invalid heap size configuration: HWMALLOC_TINY_LIMIT < HWMALLOC_SMALL_LIMIT < HWMALLOC_LARGE_LIMIT must hold. ";
        os << "Got HWMALLOC_TINY_LIMIT=" << m_tiny_limit
           << ", HWMALLOC_SMALL_LIMIT=" << m_small_limit
           << ", HWMALLOC_LARGE_LIMIT=" << m_large_limit << ".";
        throw std::runtime_error(os.str());
    }

    // Validate that limits are at most segment sizes
    if (!(tiny_limit <= tiny_segment_size && small_limit <= small_segment_size &&
            large_limit <= large_segment_size))
    {
        std::ostringstream os;
        os << "Invalid heap size configuration: Limits must be at most segment sizes. ";
        os << "Got HWMALLOC_TINY_LIMIT=" << m_tiny_limit
           << ", HWMALLOC_TINY_SEGMENT_SIZE=" << m_tiny_segment_size
           << ", HWMALLOC_SMALL_LIMIT=" << m_small_limit
           << ", HWMALLOC_SMALL_SEGMENT_SIZE=" << m_small_segment_size
           << ", HWMALLOC_LARGE_LIMIT=" << m_large_limit
           << ", HWMALLOC_LARGE_SEGMENT_SIZE=" << m_large_segment_size << ".";
        throw std::runtime_error(os.str());
    }
}

heap_config const&
get_default_heap_config()
{
    static heap_config config{
        detail::get_env<bool>("HWMALLOC_NEVER_FREE", heap_config::never_free_default),
        detail::get_env<std::size_t>("HWMALLOC_NUM_RESERVE_SEGMENTS",
            heap_config::num_reserve_segments_default),
        detail::get_env<std::size_t>("HWMALLOC_TINY_LIMIT", heap_config::tiny_limit_default),
        detail::get_env<std::size_t>("HWMALLOC_SMALL_LIMIT", heap_config::small_limit_default),
        detail::get_env<std::size_t>("HWMALLOC_LARGE_LIMIT", heap_config::large_limit_default),
        detail::get_env<std::size_t>("HWMALLOC_TINY_SEGMENT_SIZE",
            heap_config::tiny_segment_size_default),
        detail::get_env<std::size_t>("HWMALLOC_SMALL_SEGMENT_SIZE",
            heap_config::small_segment_size_default),
        detail::get_env<std::size_t>("HWMALLOC_LARGE_SEGMENT_SIZE",
            heap_config::large_segment_size_default)

    };

    return config;
}
} // namespace hwmalloc
