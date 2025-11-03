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
// These should change accordingly if the defaults in heap_config.hpp change.
// They are defined here for convencience for tests. They are derived from
// heap_config defaults.
inline constexpr std::size_t bucket_shift_default = 7u;
inline constexpr std::size_t tiny_increment_shift_default = 3u;
inline constexpr std::size_t tiny_increment_default = 8u;
inline constexpr std::size_t num_tiny_heaps_default = 16u;
inline constexpr std::size_t num_small_heaps_default = 5u;
inline constexpr std::size_t num_large_heaps_default = 9u;
} // namespace hwmalloc::test
