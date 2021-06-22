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

#include <hwmalloc/ucx/config.hpp>
#include <ucp/api/ucp.h>

#ifdef NDEBUG
#define HWMALLOC_CHECK_UCX_RESULT(x) x;
#else
#include <string>
#include <stdexcept>
#define HWMALLOC_CHECK_UCX_RESULT(x)                                                               \
    if (x != UCS_OK)                                                                               \
        throw std::runtime_error("HWMALLOC Error: UCX Call failed " + std::string(#x) + " in " +   \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));
#define HWMALLOC_CHECK_UCX_RESULT_NOEXCEPT(x)                                                      \
    if (x != UCX_OK)                                                                               \
    {                                                                                              \
        std::cerr << "HWMALLOC Error: UCX Call failed " << std::string(#x) << " in "               \
                  << std::string(__FILE__) << ":" << std::to_string(__LINE__) << std::endl;        \
        std::terminate();                                                                          \
    }
#endif
