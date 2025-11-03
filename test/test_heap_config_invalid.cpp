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

#include <heap_config_defaults.hpp>

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
    EXPECT_EQ(config.m_tiny_limit, hwmalloc::heap_config::tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, 8192);
    EXPECT_EQ(config.m_large_limit, hwmalloc::heap_config::large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, hwmalloc::test::bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, 16384u);
    EXPECT_EQ(config.m_small_segment_size, hwmalloc::heap_config::small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, 4194304u);
    EXPECT_EQ(config.m_tiny_increment_shift, hwmalloc::test::tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, hwmalloc::test::tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, hwmalloc::test::num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, 6u);
    EXPECT_EQ(config.m_num_large_heaps, 8u);
}
