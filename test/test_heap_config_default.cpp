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

// Check that we can change the default configuration values through environment
// variables.
TEST(config, defaults)
{
    ::setenv("HWMALLOC_NEVER_FREE", "1", 1);
    ::setenv("HWMALLOC_NUM_RESERVE_SEGMENTS", "19", 1);
    ::setenv("HWMALLOC_TINY_LIMIT", "512", 1);
    ::setenv("HWMALLOC_SMALL_LIMIT", "2000", 1);
    ::setenv("HWMALLOC_LARGE_LIMIT", "131072", 1);
    ::setenv("HWMALLOC_TINY_SEGMENT_SIZE", "16384", 1);
    ::setenv("HWMALLOC_SMALL_SEGMENT_SIZE", "32768", 1);
    ::setenv("HWMALLOC_LARGE_SEGMENT_SIZE", "262144", 1);

    hwmalloc::heap_config config = hwmalloc::get_default_heap_config();

    EXPECT_EQ(config.m_never_free, true);
    EXPECT_EQ(config.m_num_reserve_segments, 19u);
    EXPECT_EQ(config.m_tiny_limit, 512u);
    EXPECT_EQ(config.m_small_limit, 2048);
    EXPECT_EQ(config.m_large_limit, 131072u);
    EXPECT_EQ(config.m_bucket_shift, 9u);
    EXPECT_EQ(config.m_tiny_segment_size, 16384u);
    EXPECT_EQ(config.m_small_segment_size, 32768u);
    EXPECT_EQ(config.m_large_segment_size, 262144u);
    EXPECT_EQ(config.m_tiny_increment_shift, 3u);
    EXPECT_EQ(config.m_tiny_increment, 8u);
    EXPECT_EQ(config.m_num_tiny_heaps, 64u);
    EXPECT_EQ(config.m_num_small_heaps, 2u);
    EXPECT_EQ(config.m_num_large_heaps, 6u);
}
