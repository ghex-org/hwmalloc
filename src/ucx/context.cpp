/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/ucx/context.hpp>

namespace hwmalloc
{
namespace ucx
{
region::region(ucp_context_h ctxt, void* ptr, std::size_t size, bool gpu)
: m_ucp_context{ctxt}
, m_ptr{ptr}
, m_size{size}
{
    ucp_mem_map_params_t params;

    // enable fields
    /* clang-format off */
    params.field_mask =
          UCP_MEM_MAP_PARAM_FIELD_ADDRESS         // enable address field
        | UCP_MEM_MAP_PARAM_FIELD_LENGTH          // enable length field
    //  | UCP_MEM_MAP_PARAM_FIELD_FLAGS           // enable flags field
#if (UCP_API_VERSION >= 17432576) // version >= 1.10
        | UCP_MEM_MAP_PARAM_FIELD_MEMORY_TYPE     // enable memory type field
#endif
        ;
    /* clang-format on */

    // set fields
    params.address = ptr;
    params.length = size;
    // from version 1.10
#if (UCP_API_VERSION >= 17432576) // version >= 1.10
    params.memory_type = UCS_MEMORY_TYPE_HOST;
#endif

    // special treatment for gpu memory
#if HWMALLOC_ENABLE_DEVICE | !defined(HWMALLOC_DEVICE_EMULATE)
    if (gpu)
    {
#if (UCP_API_VERSION >= 17432576) // version >= 1.10
#if defined(HWMALLOC_DEVICE_CUDA)
        params.memory_type = UCS_MEMORY_TYPE_CUDA;
#elif defined(HWMALLOC_DEVICE_HIP)
        params.memory_type = UCS_MEMORY_TYPE_ROCM
#endif
#endif
    }
#endif

    // register memory
    HWMALLOC_CHECK_UCX_RESULT(ucp_mem_map(m_ucp_context, &params, &m_memh));
}

region
register_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size);
}

#if HWMALLOC_ENABLE_DEVICE
region
register_device_memory(context& c, void* ptr, std::size_t size)
{
    return c.make_region(ptr, size, true);
}
#endif

} // namespace ucx
} // namespace hwmalloc
