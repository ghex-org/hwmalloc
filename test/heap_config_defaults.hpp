/*
 * ghex-org
 *
 * Copyright (c) 2014-2025, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <cstddef>

namespace hwmalloc::test
{
// These should change accordingly if the defaults in heap_config.cpp change.
// They are defined here for convencience for tests.

// These can be set directly.
inline constexpr bool        never_free_default = false;
inline constexpr std::size_t num_reserve_segments_default = 16u;
inline constexpr std::size_t tiny_limit_default = 128u;
inline constexpr std::size_t small_limit_default = 4096u;
inline constexpr std::size_t large_limit_default = 2097152u;
inline constexpr std::size_t tiny_segment_size_default = 65536u;
inline constexpr std::size_t small_segment_size_default = 65536u;
inline constexpr std::size_t large_segment_size_default = 2097152u;

// These are derived from above.
inline constexpr std::size_t bucket_shift_default = 7u;
inline constexpr std::size_t tiny_increment_shift_default = 3u;
inline constexpr std::size_t tiny_increment_default = 8u;
inline constexpr std::size_t num_tiny_heaps_default = 16u;
inline constexpr std::size_t num_small_heaps_default = 5u;
inline constexpr std::size_t num_large_heaps_default = 9u;
} // namespace hwmalloc::test
