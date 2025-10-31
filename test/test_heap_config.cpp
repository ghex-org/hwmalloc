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

#include <cstddef>

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

// Check that the default configuration values are as expected.
TEST(heap_config, defaults)
{
    hwmalloc::heap_config config = hwmalloc::get_default_heap_config();

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

// We try changing one parameter a time compared to the defaults and check that
// the derived values change accordingly.
TEST(heap_config, never_free)
{
    hwmalloc::heap_config config{true, num_reserve_segments_default, tiny_limit_default,
        small_limit_default, large_limit_default, tiny_segment_size_default,
        small_segment_size_default, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, true); // This changes to the literal value we set
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

TEST(heap_config, num_reserve_segments)
{
    hwmalloc::heap_config config{never_free_default, 7, tiny_limit_default, small_limit_default,
        large_limit_default, tiny_segment_size_default, small_segment_size_default,
        large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, 7u); // This changes to the literal value we set
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

TEST(heap_config, tiny_limit)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default, 256u,
        small_limit_default, large_limit_default, tiny_segment_size_default,
        small_segment_size_default, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, 256u); // This changes to the literal value we set
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, 8u); // This changes based on tiny_limit
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, 32u); // This number doubles...
    EXPECT_EQ(config.m_num_small_heaps, 4u); // While this is decreased by one
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

TEST(heap_config, tiny_segment)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default,
        tiny_limit_default, small_limit_default, large_limit_default, 8192u,
        small_segment_size_default, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, 8192u); // This changes to the literal value we set
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

TEST(heap_config, small_limit)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default,
        tiny_limit_default, 8192u, large_limit_default, tiny_segment_size_default,
        small_segment_size_default, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, 8192u); // This changes to the literal value we set
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, 6u); // This is log2(8192)-log2(128) = 6...
    EXPECT_EQ(config.m_num_large_heaps, 8u); // While this is decreased accordingly
}

TEST(heap_config, small_segment)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default,
        tiny_limit_default, small_limit_default, large_limit_default, tiny_segment_size_default,
        16384u, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, 16384u); // This changes to the literal value we set
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

TEST(heap_config, large_limit)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default,
        tiny_limit_default, small_limit_default, 32768u, tiny_segment_size_default,
        small_segment_size_default, large_segment_size_default};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, 32768u); // This changes to the literal value we set
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, large_segment_size_default);
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, 3u); // This is decreased accordingly
}

TEST(heap_config, large_segment)
{
    hwmalloc::heap_config config{never_free_default, num_reserve_segments_default,
        tiny_limit_default, small_limit_default, large_limit_default, tiny_segment_size_default,
        small_segment_size_default, 4194304u};

    EXPECT_EQ(config.m_never_free, never_free_default);
    EXPECT_EQ(config.m_num_reserve_segments, num_reserve_segments_default);
    EXPECT_EQ(config.m_tiny_limit, tiny_limit_default);
    EXPECT_EQ(config.m_small_limit, small_limit_default);
    EXPECT_EQ(config.m_large_limit, large_limit_default);
    EXPECT_EQ(config.m_bucket_shift, bucket_shift_default);
    EXPECT_EQ(config.m_tiny_segment_size, tiny_segment_size_default);
    EXPECT_EQ(config.m_small_segment_size, small_segment_size_default);
    EXPECT_EQ(config.m_large_segment_size, 4194304u); // This changes to the literal value we set
    EXPECT_EQ(config.m_tiny_increment_shift, tiny_increment_shift_default);
    EXPECT_EQ(config.m_tiny_increment, tiny_increment_default);
    EXPECT_EQ(config.m_num_tiny_heaps, num_tiny_heaps_default);
    EXPECT_EQ(config.m_num_small_heaps, num_small_heaps_default);
    EXPECT_EQ(config.m_num_large_heaps, num_large_heaps_default);
}

// Check that setting non-power-of-two values results in rounding to power of two
TEST(heap_config, power_of_two)
{
    hwmalloc::heap_config config{never_free_default, 17u, 200u, 500u, 20000u, 1000u, 3000u,
        100000u};

    EXPECT_EQ(config.m_num_reserve_segments, 17u);   // Not rounded up
    EXPECT_EQ(config.m_tiny_limit, 256u);            // Rounded up from 200
    EXPECT_EQ(config.m_small_limit, 512u);           // Rounded up from 500
    EXPECT_EQ(config.m_large_limit, 32768u);         // Rounded up from 20000
    EXPECT_EQ(config.m_tiny_segment_size, 1024u);    // Rounded up from 1000
    EXPECT_EQ(config.m_small_segment_size, 4096u);   // Rounded up from 3000
    EXPECT_EQ(config.m_large_segment_size, 131072u); // Rounded up from 100000
}

// Check that setting invalid values results in exceptions
TEST(heap_config, validate_tiny_small_limit)
{
    EXPECT_THROW((hwmalloc::heap_config{never_free_default, num_reserve_segments_default, 2048u,
                     1024u, large_limit_default, tiny_segment_size_default,
                     small_segment_size_default, large_segment_size_default}),
        std::runtime_error);
}

TEST(heap_config, validate_small_large_limit)
{
    EXPECT_THROW((hwmalloc::heap_config{never_free_default, num_reserve_segments_default,
                     tiny_limit_default, 131072u, 65536u, tiny_segment_size_default,
                     small_segment_size_default, large_segment_size_default}),
        std::runtime_error);
}

TEST(heap_config, validate_tiny_limit_segment_size)
{
    EXPECT_THROW((hwmalloc::heap_config{never_free_default, num_reserve_segments_default, 2048u,
                     small_limit_default, large_limit_default, 1024u, small_segment_size_default,
                     large_segment_size_default}),
        std::runtime_error);
}

TEST(heap_config, validate_small_limit_segment_size)
{
    EXPECT_THROW((hwmalloc::heap_config{never_free_default, num_reserve_segments_default,
                     tiny_limit_default, 131072u, large_limit_default, tiny_segment_size_default,
                     65536u, large_segment_size_default}),
        std::runtime_error);
}

TEST(heap_config, validate_large_limit_segment_size)
{
    EXPECT_THROW((hwmalloc::heap_config{never_free_default, num_reserve_segments_default,
                     tiny_limit_default, small_limit_default, 262144u, tiny_segment_size_default,
                     small_segment_size_default, 131072u}),
        std::runtime_error);
}
