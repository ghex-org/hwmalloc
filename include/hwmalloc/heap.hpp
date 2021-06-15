/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <hwmalloc/detail/fixed_size_heap.hpp>
#include <hwmalloc/fancy_ptr/void_ptr.hpp>
#include <hwmalloc/fancy_ptr/const_void_ptr.hpp>
#include <hwmalloc/fancy_ptr/unique_ptr.hpp>
#include <hwmalloc/allocator.hpp>
#include <vector>
#include <unordered_map>

namespace hwmalloc
{
// Main class of this library. Provides a heap for allocating memory on given numa nodes and
// devices. The memory is requested from the OS/runtime in large segments which are kept alive.
// After allocation of these segments, the memory is given to the Context for registering with e.g.
// a network transport layer. This is achieved through customization point objects( see register.hpp
// and register_device.hpp).
//
// This class is thread safe. The requested memory is returned in form of fancy pointers which store
// additional information about the chunk of memory (segment) from which they originated and provide
// access to potential RMA keys that the network layer generates during registering.
//
// If device (gpu) memory is requested, space will be allocated on both the device and the host
// (effectively mirroring the memory). Both memory regions are passed to the Context for
// registration. Note, that setting a numa node for device memory allocation is therefore still
// necessary.
template<typename Context>
class heap
{
  public:
    using this_type = heap<Context>;
    using fixed_size_heap_type = detail::fixed_size_heap<Context>;
    using block_type = typename fixed_size_heap_type::block_type;
    using heap_vector = std::vector<std::unique_ptr<fixed_size_heap_type>>;
    using heap_map = std::unordered_map<std::size_t, std::unique_ptr<fixed_size_heap_type>>;
    using pointer = hw_void_ptr<block_type>;
    using const_pointer = hw_const_void_ptr<block_type>;
    template<typename T>
    using allocator_type = allocator<T, this_type>;

    // There are 5 size classes that the heap uses. For each size class it relies on a
    // fixed_size_heap. The size classes are:
    // - tiny:  heaps with linearly increasing block sizes, each heap backed by 16KiB segments
    // - small: heaps with exponentially increasing block sizes, each heap backed by 32KiB segments
    // - large: heaps with exponentially increasing block sizes, each heap backed by 64KiB segments
    // - huge:  heaps with exponentially increasing block sizes, each heap backed by segments of
    //          size = block size
    // - Huge:  heaps with exponentially increasing block sizes, each heap backed by segments of
    //          size = block size. These heaps can use arbitrary large block sizes and are stored
    //          in a map (created on demand). Access is synchronized among threads using a mutex.
    //          TODO: can this be made more efficient?
    //
    //     block  segment / pages / h / hex      blocks/segment
    //   ------------------------------------------------------ tiny
    //         8   16384 /  4 / 16KiB / 0x04000            2048  |        -+
    //        16   16384 /  4 / 16KiB / 0x04000            1024  v         |
    //        24   16384 /  4 / 16KiB / 0x04000             682            |
    //        32   16384 /  4 / 16KiB / 0x04000             512            |  m_tiny_heaps: vector
    //        40   16384 /  4 / 16KiB / 0x04000             409            |
    //        48   16384 /  4 / 16KiB / 0x04000             341            |
    //         :                                                           :
    //       128   16384 /  4 / 16KiB / 0x04000             128           -+
    //   ------------------------------------------------------ small
    //       256   32768 /  8 / 32KiB / 0x08000             128  |        -+
    //       512   32768 /  8 / 32KiB / 0x08000              64  v         |
    //      1024   32768 /  8 / 32KiB / 0x08000              32            |
    //   ------------------------------------------------------ large      |
    //      2048   65536 / 16 / 64KiB / 0x10000              32  |         |
    //      4096   65536 / 16 / 64KiB / 0x10000              16  v         |
    //      8192   65536 / 16 / 64KiB / 0x10000               8            |  m_heaps: vector
    //     16384   65536 / 16 / 64KiB / 0x10000               4            |
    //     32768   65536 / 16 / 64KiB / 0x10000               2            |
    //     65536   65536 / 16 / 64KiB / 0x10000               1            |
    //   ------------------------------------------------------ huge       |
    //    131072  131072 / 32 / 128KiB                        1  |         |
    //    :                                                      v         |
    //    max_size                                            1           -+
    //  -------------------------------------------------------- Huge
    //    stored in map                                                   -+
    //    :                                                                :  m_huge_heaps: map

  private:
    static constexpr std::size_t log2_c(std::size_t n) noexcept
    {
        return ((n < 2) ? 1 : 1 + log2_c(n >> 1));
    }

    static constexpr std::size_t s_tiny_limit = (1u << 7);   //   128
    static constexpr std::size_t s_small_limit = (1u << 10); //  1024
    static constexpr std::size_t s_large_limit = (1u << 16); // 65536

    static constexpr std::size_t s_bucket_shift = log2_c(s_tiny_limit) - 1;

    static constexpr std::size_t s_tiny_segment = 0x04000;  // 16KiB
    static constexpr std::size_t s_small_segment = 0x08000; // 32KiB
    static constexpr std::size_t s_large_segment = 0x10000; // 64KiB

    static constexpr std::size_t s_tiny_increment_shift = 3;
    static constexpr std::size_t s_tiny_increment = (1u << s_tiny_increment_shift); // = 8

    static constexpr std::size_t s_num_tiny_heaps = s_tiny_limit / s_tiny_increment;
    static constexpr std::size_t s_num_small_heaps = log2_c(s_small_limit) - log2_c(s_tiny_limit);
    static constexpr std::size_t s_num_large_heaps = log2_c(s_large_limit) - log2_c(s_small_limit);

    static constexpr std::size_t tiny_bucket_index(std::size_t n) noexcept
    {
        return (n + s_tiny_increment - 1) >> s_tiny_increment_shift;
    }

    static constexpr std::size_t bucket_index(std::size_t n) noexcept
    {
        return log2_c((n - 1) >> s_bucket_shift) - 1;
    }

    static constexpr std::size_t round_to_pow_of_2(std::size_t n) noexcept
    {
        return 1u << log2_c(n - 1);
    }

  private:
    Context*    m_context;
    std::size_t m_max_size;
    bool        m_never_free;
    heap_vector m_tiny_heaps;
    heap_vector m_heaps;
    heap_map    m_huge_heaps;
    std::mutex  m_mutex;

  public:
    heap(Context* context, std::size_t max_size = s_large_limit * 2, bool never_free = false)
    : m_context{context}
    , m_max_size(std::max(round_to_pow_of_2(max_size), s_large_limit))
    , m_never_free{never_free}
    , m_tiny_heaps(s_tiny_limit / s_tiny_increment)
    , m_heaps(bucket_index(m_max_size) + 1)
    {
        for (std::size_t i = 0; i < m_tiny_heaps.size(); ++i)
            m_tiny_heaps[i] = std::make_unique<fixed_size_heap_type>(
                m_context, s_tiny_increment * (i + 1), s_tiny_segment, m_never_free);

        for (std::size_t i = 0; i < s_num_small_heaps; ++i)
            m_heaps[i] = std::make_unique<fixed_size_heap_type>(
                m_context, (s_tiny_limit << (i + 1)), s_small_segment, m_never_free);

        for (std::size_t i = 0; i < s_num_large_heaps; ++i)
            m_heaps[i + s_num_small_heaps] = std::make_unique<fixed_size_heap_type>(
                m_context, (s_small_limit << (i + 1)), s_large_segment, m_never_free);

        for (std::size_t i = 0; i < m_heaps.size() - (s_num_small_heaps + s_num_large_heaps); ++i)
            m_heaps[i + s_num_small_heaps + s_num_large_heaps] =
                std::make_unique<fixed_size_heap_type>(m_context, (s_large_limit << (i + 1)),
                    (s_large_limit << (i + 1)), m_never_free);
    }

    heap(heap const&) = delete;
    heap(heap&&) = delete;

    pointer allocate(std::size_t size, std::size_t numa_node)
    {
        if (size <= s_tiny_limit)
            return {m_tiny_heaps[tiny_bucket_index(size)]->allocate(numa_node)};
        else if (size <= m_max_size)
            return {m_heaps[bucket_index(size)]->allocate(numa_node)};
        else
        {
            fixed_size_heap_type* h;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                const auto                  s = round_to_pow_of_2(size);
                auto&                       u_ptr = m_huge_heaps[s];
                if (!u_ptr)
                    u_ptr = std::make_unique<fixed_size_heap_type>(m_context, s, s, m_never_free);
                h = u_ptr.get();
            }
            return {h->allocate(numa_node)};
        }
    }

#if HWMALLOC_ENABLE_DEVICE
    pointer allocate(std::size_t size, std::size_t numa_node, int device_id)
    {
        if (size <= s_tiny_limit)
            return {m_tiny_heaps[tiny_bucket_index(size)]->allocate(numa_node, device_id)};
        else if (size <= m_max_size)
            return {m_heaps[bucket_index(size)]->allocate(numa_node, device_id)};
        else
        {
            fixed_size_heap_type* h;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                const auto                  s = round_to_pow_of_2(size);
                auto&                       u_ptr = m_huge_heaps[s];
                if (!u_ptr)
                    u_ptr = std::make_unique<fixed_size_heap_type>(m_context, s, s, m_never_free);
                h = u_ptr.get();
            }
            return {h->allocate(numa_node, device_id)};
        }
    }
#endif

    template<typename VoidPtr>
    void free(hw_void_ptr<block_type, VoidPtr> const& ptr)
    {
        ptr.m_data.release();
    }

    template<typename T>
    allocator_type<T> get_allocator(std::size_t numa_node) noexcept
    {
        return {this, numa_node};
    }

    // scalar version
    template<typename T, typename... Args>
    std::enable_if_t<!std::is_array<T>::value, unique_ptr<T, block_type>> make_unique(
        std::size_t numa_node, Args&&... args)
    {
        auto ptr = allocate(sizeof(T), numa_node);
        new (ptr.get()) T(std::forward<Args>(args)...);
        return unique_ptr<T, block_type>(static_cast<hw_ptr<T, block_type>>(ptr));
    }

    // array version
    template<typename T>
    std::enable_if_t<std::is_array<T>::value, unique_ptr<T, block_type>> make_unique(
        std::size_t numa_node, std::size_t size)
    {
        using U = typename std::remove_extent<T>::type;
        auto ptr = allocate(sizeof(U) * size, numa_node);
        new (ptr.get()) U[size]();
        return unique_ptr<T, block_type>(
            static_cast<hw_ptr<U, block_type>>(ptr), heap_delete<T, block_type>{size});
    }
};

template<typename Context>
constexpr std::size_t heap<Context>::s_tiny_limit;
template<typename Context>
constexpr std::size_t heap<Context>::s_small_limit;
template<typename Context>
constexpr std::size_t heap<Context>::s_large_limit;
template<typename Context>
constexpr std::size_t heap<Context>::s_bucket_shift;
template<typename Context>
constexpr std::size_t heap<Context>::s_tiny_segment;
template<typename Context>
constexpr std::size_t heap<Context>::s_small_segment;
template<typename Context>
constexpr std::size_t heap<Context>::s_large_segment;
template<typename Context>
constexpr std::size_t heap<Context>::s_tiny_increment_shift;
template<typename Context>
constexpr std::size_t heap<Context>::s_tiny_increment;
template<typename Context>
constexpr std::size_t heap<Context>::s_num_tiny_heaps;
template<typename Context>
constexpr std::size_t heap<Context>::s_num_small_heaps;
template<typename Context>
constexpr std::size_t heap<Context>::s_num_large_heaps;

} // namespace hwmalloc
