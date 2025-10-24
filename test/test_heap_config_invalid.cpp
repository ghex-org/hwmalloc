/*
 * ghex-org
 *
 * Copyright (c) 2014-2025, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <gtest/gtest.h>

#include <hwmalloc/heap_config.hpp>

#include <cstdlib>

// These should change accordingly if the defaults in heap_config.cpp change.
// They are defined here for a little bit of convenience in the tests below.
inline constexpr bool        never_free_default = false;
inline constexpr std::size_t num_reserve_segments_default = 16u;
inline constexpr std::size_t tiny_limit_default = 128u;
inline constexpr std::size_t small_limit_default = 4096u;
inline constexpr std::size_t large_limit_default = 2097152u;
inline constexpr std::size_t bucket_shift_default = 7u;
inline constexpr std::size_t tiny_segment_size_default = 65536u;
inline constexpr std::size_t small_segment_size_default = 65536u;
inline constexpr std::size_t large_segment_size_default = 2097152u;
inline constexpr std::size_t tiny_increment_shift_default = 3u;
inline constexpr std::size_t tiny_increment_default = 8u;
inline constexpr std::size_t num_tiny_heaps_default = 16u;
inline constexpr std::size_t num_small_heaps_default = 5u;
inline constexpr std::size_t num_large_heaps_default = 9u;

// Test that config falls back to defaults if environment variables are given
// non-numeric values.
//
// A warning is printed if logging is enabled.
TEST(config, defaults)
{
    ::setenv("HWMALLOC_NEVER_FREE", "foo", 1);
    ::setenv("HWMALLOC_NUM_RESERVE_SEGMENTS", "9", 1);
    ::setenv("HWMALLOC_TINY_LIMIT", "abcd", 1);
    ::setenv("HWMALLOC_SMALL_LIMIT", "8192", 1);
    ::setenv("HWMALLOC_LARGE_LIMIT", "foo", 1);
    ::setenv("HWMALLOC_TINY_SEGMENT_SIZE", "16384", 1);
    ::setenv("HWMALLOC_SMALL_SEGMENT_SIZE", "bar", 1);
    ::setenv("HWMALLOC_LARGE_SEGMENT_SIZE", "4194304", 1);

    hwmalloc::heap_config config = hwmalloc::get_default_heap_config();

    EXPECT_EQ(config.m_never_free, false);
    EXPECT_EQ(config.m_num_reserve_segments, 9u);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, 8192);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, 16384u);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, 4194304u);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, 6u);
    EXPECT_EQ(config.m_num_large_heaps, 8u);
}
