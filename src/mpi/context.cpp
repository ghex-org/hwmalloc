/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/mpi/context.hpp>

namespace hwmalloc
{
namespace mpi
{
context::context(MPI_Comm comm)
: m_comm{comm}
{
    MPI_Info info;
    HWMALLOC_CHECK_MPI_RESULT(MPI_Info_create(&info));
    HWMALLOC_CHECK_MPI_RESULT(MPI_Info_set(info, "no_locks", "false"));
    HWMALLOC_CHECK_MPI_RESULT(MPI_Win_create_dynamic(info, m_comm, &m_win));
    MPI_Info_free(&info);
    //MPI_Win_create_dynamic(MPI_INFO_NULL, m_comm, &m_win);
    HWMALLOC_CHECK_MPI_RESULT(MPI_Win_fence(0,m_win));
}

context::~context() { MPI_Win_free(&m_win); }

region
context::make_region(void* ptr, std::size_t size) const
{
    return {m_comm, m_win, ptr, size};
}

region
register_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size);
}

} // namespace mpi
} // namespace hwmalloc
