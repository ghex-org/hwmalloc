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

#include <hwmalloc/heap.hpp>
#include <mpi.h>
#include <iostream>

namespace hwmalloc
{
namespace mpi
{
struct handle
{
    void*       m_ptr;
    std::size_t m_size;

    MPI_Aint get_remote_key() const noexcept
    {
        MPI_Aint address;
        MPI_Get_address(m_ptr, &address);
        return address;
        //return ((char*)m_ptr - MPI_BOTTOM);
    }
};

struct region
{
    using handle_type = handle;
    MPI_Comm m_comm;
    MPI_Win  m_win;
    void*    m_ptr;

    region(MPI_Comm comm, MPI_Win win, void* ptr, std::size_t size)
    : m_comm{comm}
    , m_win{win}
    , m_ptr{ptr}
    {
        // attach ptr to window
        std::cout << "attaching memory " << ptr << " with size " << size << " to window"
                  << std::endl;
        MPI_Win_attach(m_win, ptr, size);
    }

    region(region const&) = delete;

    region(region&& r) noexcept
    : m_comm{r.m_comm}
    , m_win{r.m_win}
    , m_ptr{std::exchange(r.m_ptr, nullptr)}
    {
    }

    ~region()
    {
        if (m_ptr)
        {
            // detach memory from window
            std::cout << "detaching memory " << m_ptr << " from window" << std::endl;
            MPI_Win_detach(m_win, m_ptr);
        }
    }

    // get a handle to some portion of the region
    handle_type get_handle(std::size_t offset, std::size_t size)
    {
        return {(void*)((char*)m_ptr + offset), size};
    }
};

class context
{
    MPI_Comm m_comm;
    MPI_Win  m_win;

  public:
    context(MPI_Comm comm)
    : m_comm{comm}
    {
        MPI_Info info;
        MPI_Info_create(&info);
        MPI_Info_set(info, "no_locks", "false");
        MPI_Win_create_dynamic(info, m_comm, &m_win);
        MPI_Info_free(&info);
        //MPI_Win_create_dynamic(MPI_INFO_NULL, m_comm, &m_win);
    }

    context(context const&) = delete;
    context(context&&) = delete;

    ~context() { MPI_Win_free(&m_win); }

    auto make_region(void* ptr, std::size_t size) { return region{m_comm, m_win, ptr, size}; }
};

auto
register_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size);
}

using heap = ::hwmalloc::heap<context>;

} // namespace mpi
} // namespace hwmalloc
