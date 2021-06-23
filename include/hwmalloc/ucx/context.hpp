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
#include <hwmalloc/ucx/error.hpp>

namespace hwmalloc
{
namespace ucx
{
struct handle
{
    void*       m_ptr;
    std::size_t m_size;
};

struct region
{
    using handle_type = handle;
    ucp_context_h m_ucp_context;
    void*         m_ptr;
    std::size_t   m_size;
    ucp_mem_h     m_memh;

    region(ucp_context_h ctxt, void* ptr, std::size_t size)
    : m_ucp_context{ctxt}
    , m_ptr{ptr}
    , m_size{size}
    {
        ucp_mem_map_params_t params;

        params.field_mask = UCP_MEM_MAP_PARAM_FIELD_ADDRESS | UCP_MEM_MAP_PARAM_FIELD_LENGTH
            //| UCP_MEM_MAP_PARAM_FIELD_FLAGS
            //| UCP_MEM_MAP_PARAM_FIELD_MEMORY_TYPE
            ;

        params.address = ptr;
        params.length = size;

        //ucs_status_t
        HWMALLOC_CHECK_UCX_RESULT(ucp_mem_map(m_ucp_context, &params, &m_memh));
    }

    region(region const&) = delete;

    region(region&& r) noexcept
    : m_ucp_context{r.m_ucp_context}
    , m_ptr{std::exchange(r.m_ptr, nullptr)}
    , m_size{r.m_size}
    , m_memh{r.m_memh}
    {
    }

    ~region()
    {
        if (m_ptr) { ucp_mem_unmap(m_ucp_context, m_memh); }
    }

    // get a handle to some portion of the region
    handle_type get_handle(std::size_t offset, std::size_t size)
    {
        return {(void*)((char*)m_ptr + offset), size};
    }
};

struct context
{
    ucp_context_h m_ucp_context;

    context(ucp_context_h ctxt) noexcept
    : m_ucp_context{ctxt}
    {
    }
    context(context const&) = delete;
    context(context&&) = delete;
    ~context() {}

    region make_region(void* ptr, std::size_t size) { return {m_ucp_context, ptr, size}; }
};

region register_memory(context& c, void* ptr, std::size_t size);

} // namespace ucx
} // namespace hwmalloc
