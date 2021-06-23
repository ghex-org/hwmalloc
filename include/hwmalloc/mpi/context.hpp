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

#include <hwmalloc/register.hpp>
#include <hwmalloc/mpi/error.hpp>

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
        HWMALLOC_CHECK_MPI_RESULT_NOEXCEPT(MPI_Get_address(m_ptr, &address));
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
        HWMALLOC_CHECK_MPI_RESULT(MPI_Win_attach(m_win, ptr, size));
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
        if (m_ptr) MPI_Win_detach(m_win, m_ptr);
    }

    // get a handle to some portion of the region
    handle_type get_handle(std::size_t offset, std::size_t size)
    {
        return {(void*)((char*)m_ptr + offset), size};
    }
};

class context
{
  private:
    MPI_Comm m_comm;
    MPI_Win  m_win;

  public:
    context(MPI_Comm comm);
    context(context const&) = delete;
    context(context&&) = delete;
    ~context();
    region make_region(void* ptr, std::size_t size) const;

    auto get_window() const noexcept { return m_win; }
};

auto
register_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size);
}

} // namespace mpi
} // namespace hwmalloc
