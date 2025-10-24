/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <gtest/gtest.h>

#include <hwmalloc/detail/segment.hpp>
#include <hwmalloc/detail/pool.hpp>
#include <hwmalloc/detail/fixed_size_heap.hpp>
#include <hwmalloc/heap.hpp>

#include <thread>
#include <cstring>
#include <omp.h>
#include <numa.h>

int NBUFFERS = 10;
int BUFFSIZE = 1024 * 1024 * 8;
int NITER = 10;

struct context
{
    int m = 42;
    context() {}
    ~context() {}

    struct region
    {
        struct handle_type
        {
            void* ptr;
        };

        void* ptr = nullptr;

        region(void* p) noexcept
        : ptr{p}
        {
        }

        region(region const&) = delete;

        region(region&& other) noexcept
        : ptr{std::exchange(other.ptr, nullptr)}
        {
        }

        ~region() {}

        handle_type get_handle(std::size_t offset, std::size_t /*size*/) const noexcept
        {
            return {(void*)((char*)ptr + offset)};
        }
    };
};

int n_registrations = 0;

auto
register_memory(context&, void* ptr, std::size_t size)
{
#pragma omp atomic
    n_registrations++;
    std::memset(ptr, 0, size);
    return context::region{ptr};
}

TEST(spread, neverfree)
{
    using heap_t = hwmalloc::heap<context>;
    using heap_config_t = hwmalloc::heap_config;
    context c;

    heap_config_t hc = hwmalloc::get_default_heap_config();
    hc.m_never_free = true;
    hc.m_num_reserve_segments = 1024;
    heap_t h(&c, hc);
    n_registrations = 0;

#pragma omp parallel
    {
        int nthr = omp_get_num_threads();
        int thrid = omp_get_thread_num();

        std::vector<heap_t::pointer> pointers;
        pointers.resize(NBUFFERS);

        bitmask* nodemask;
        int      nnodes = hwmalloc::numa().local_nodes().size();
        int      thr_per_node = nthr / nnodes;
        int      local_node = thrid % nnodes;

        if (thr_per_node == 0)
        {
            nnodes = nthr;
            thr_per_node++;
        }

#pragma omp master
        {
            printf("SPREAD, never_free :: %d theads per numa node, %d numa nodes\n", thr_per_node,
                nnodes);
            EXPECT_TRUE((nthr == nnodes) || (nthr % nnodes == 0));
        }

        nodemask = numa_allocate_nodemask();
        numa_bitmask_setbit(nodemask, local_node);
        numa_bind(nodemask);
        numa_free_nodemask(nodemask);
        // printf("%d local node %d (%d)\n", thrid, local_node, hwmalloc::numa().local_node());

        for (int i = 0; i < NITER; i++)
        {
            // execute thread by thread
            for (int tid = 0; tid < nthr; tid++)
            {
#pragma omp barrier
                if (tid == thrid)
                {
                    for (int j = 0; j < NBUFFERS; j++)
                    {
                        pointers[j] = h.allocate(BUFFSIZE, hwmalloc::numa().local_node());
                        if (hwmalloc::numa().get_node(pointers[j].get()) !=
                            hwmalloc::numa().local_node())
                        {
                            EXPECT_TRUE(hwmalloc::numa().get_node(pointers[j].get()) ==
                                        hwmalloc::numa().local_node());
                        }
                    }
                    for (int j = 0; j < NBUFFERS; j++) { h.free(pointers[j]); }
                }
            }
        }

#pragma omp barrier
#pragma omp master
        {
            printf("SPREAD, never_free :: n_registrations %d\n", n_registrations);
            EXPECT_TRUE(n_registrations == nnodes * NBUFFERS);
        }
    }
}

TEST(close, neverfree)
{
    using heap_t = hwmalloc::heap<context>;
    using heap_config_t = hwmalloc::heap_config;
    context c;

    heap_config_t hc = hwmalloc::get_default_heap_config();
    hc.m_never_free = true;
    hc.m_num_reserve_segments = 1024;
    heap_t h(&c, hc);
    n_registrations = 0;

#pragma omp parallel
    {
        int nthr = omp_get_num_threads();
        int thrid = omp_get_thread_num();

        std::vector<heap_t::pointer> pointers;
        pointers.resize(NBUFFERS);

        struct bitmask* mask = numa_allocate_cpumask();
        numa_node_to_cpus(0, mask);
        int ncpus_per_node = numa_bitmask_weight(mask);
        numa_free_cpumask(mask);
        int nused_nodes = nthr / ncpus_per_node + 1;
        if (nthr % ncpus_per_node == 0) nused_nodes--;
        int local_node = thrid % nused_nodes;

#pragma omp master
        {
            printf("CLOSE, never_free :: %d cpus per node, %d numa nodes\n", ncpus_per_node,
                nused_nodes);
            EXPECT_TRUE((nused_nodes == 1) || (nthr == nused_nodes * ncpus_per_node));
        }

        mask = numa_allocate_nodemask();
        numa_bitmask_setbit(mask, local_node);
        numa_bind(mask);
        numa_free_nodemask(mask);
        // printf("%d local node %d (%d)\n", thrid, local_node, hwmalloc::numa().local_node());

        for (int i = 0; i < NITER; i++)
        {
            // execute thread by thread
            for (int tid = 0; tid < nthr; tid++)
            {
#pragma omp barrier
                if (tid == thrid)
                {
                    for (int j = 0; j < NBUFFERS; j++)
                    {
                        pointers[j] = h.allocate(BUFFSIZE, hwmalloc::numa().local_node());
                        if (hwmalloc::numa().get_node(pointers[j].get()) !=
                            hwmalloc::numa().local_node())
                        {
                            EXPECT_TRUE(hwmalloc::numa().get_node(pointers[j].get()) ==
                                        hwmalloc::numa().local_node());
                        }
                    }
                    for (int j = 0; j < NBUFFERS; j++) { h.free(pointers[j]); }
                }
            }
        }

#pragma omp barrier
#pragma omp master
        {
            printf("CLOSE, never_free :: n_registrations %d\n", n_registrations);
            EXPECT_TRUE(n_registrations == nused_nodes * NBUFFERS);
        }
    }
}

TEST(spread, free)
{
    using heap_t = hwmalloc::heap<context>;
    context c;
    heap_t  h(&c);
    n_registrations = 0;

#pragma omp parallel
    {
        int nthr = omp_get_num_threads();
        int thrid = omp_get_thread_num();

        std::vector<heap_t::pointer> pointers;
        pointers.resize(NBUFFERS);

        struct bitmask* nodemask;
        int             nnodes = hwmalloc::numa().local_nodes().size();
        int             thr_per_node = nthr / nnodes;
        int             local_node = thrid % nnodes;

        if (thr_per_node == 0)
        {
            nnodes = nthr;
            thr_per_node++;
        }

#pragma omp master
        {
            printf(
                "SPREAD, free :: %d theads per numa node, %d numa nodes\n", thr_per_node, nnodes);
            EXPECT_TRUE((nthr == nnodes) || (nthr % nnodes == 0));
        }

        nodemask = numa_allocate_nodemask();
        numa_bitmask_setbit(nodemask, local_node);
        numa_bind(nodemask);
        numa_free_nodemask(nodemask);
        // printf("%d local node %d (%d)\n", thrid, local_node, hwmalloc::numa().local_node());

        for (int i = 0; i < NITER; i++)
        {
            // execute thread by thread
            for (int tid = 0; tid < nthr; tid++)
            {
#pragma omp barrier
                if (tid == thrid)
                {
                    for (int j = 0; j < NBUFFERS; j++)
                    {
                        pointers[j] = h.allocate(BUFFSIZE, hwmalloc::numa().local_node());
                        if (hwmalloc::numa().get_node(pointers[j].get()) !=
                            hwmalloc::numa().local_node())
                        {
                            EXPECT_TRUE(hwmalloc::numa().get_node(pointers[j].get()) ==
                                        hwmalloc::numa().local_node());
                        }
                    }
                    for (int j = 0; j < NBUFFERS; j++) { h.free(pointers[j]); }
                }
            }
        }

#pragma omp barrier
#pragma omp master
        {
            printf("SPREAD, free :: n_registrations %d\n", n_registrations);
            EXPECT_TRUE(
                n_registrations == (thr_per_node * NITER * nnodes * (NBUFFERS - 1) + nnodes));
        }
    }
}

TEST(close, free)
{
    using heap_t = hwmalloc::heap<context>;
    context c;
    heap_t  h(&c);
    n_registrations = 0;

#pragma omp parallel
    {
        int nthr = omp_get_num_threads();
        int thrid = omp_get_thread_num();

        std::vector<heap_t::pointer> pointers;
        pointers.resize(NBUFFERS);

        struct bitmask* mask = numa_allocate_cpumask();
        numa_node_to_cpus(0, mask);
        int ncpus_per_node = numa_bitmask_weight(mask);
        numa_free_cpumask(mask);
        int nused_nodes = nthr / ncpus_per_node + 1;
        if (nthr % ncpus_per_node == 0) nused_nodes--;
        int local_node = thrid % nused_nodes;

#pragma omp master
        {
            printf("CLOSE, free :: %d cpus per node, %d numa nodes\n", ncpus_per_node, nused_nodes);
            EXPECT_TRUE((nused_nodes == 1) || (nthr == nused_nodes * ncpus_per_node));
        }

        mask = numa_allocate_nodemask();
        numa_bitmask_setbit(mask, local_node);
        numa_bind(mask);
        numa_free_nodemask(mask);
        // printf("%d local node %d (%d)\n", thrid, local_node, hwmalloc::numa().local_node());

        for (int i = 0; i < NITER; i++)
        {
            // execute thread by thread
            for (int tid = 0; tid < nthr; tid++)
            {
#pragma omp barrier
                if (tid == thrid)
                {
                    for (int j = 0; j < NBUFFERS; j++)
                    {
                        pointers[j] = h.allocate(BUFFSIZE, hwmalloc::numa().local_node());
                        if (hwmalloc::numa().get_node(pointers[j].get()) !=
                            hwmalloc::numa().local_node())
                        {
                            EXPECT_TRUE(hwmalloc::numa().get_node(pointers[j].get()) ==
                                        hwmalloc::numa().local_node());
                        }
                    }
                    for (int j = 0; j < NBUFFERS; j++) { h.free(pointers[j]); }
                }
            }
        }

#pragma omp barrier
#pragma omp master
        {
            printf("CLOSE, free :: n_registrations %d\n", n_registrations);
            EXPECT_TRUE(n_registrations == (nthr * NITER * (NBUFFERS - 1) + nused_nodes));
        }
    }
}
