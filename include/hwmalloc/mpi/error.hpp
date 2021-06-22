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

#include <hwmalloc/mpi/config.hpp>
#include <mpi.h>

#ifdef NDEBUG
#define HWMALLOC_CHECK_MPI_RESULT(x) x;
#define HWMALLOC_CHECK_MPI_RESULT_NOEXCEPT(x) x;
#else
#include <string>
#include <stdexcept>
#include <iostream>
#define HWMALLOC_CHECK_MPI_RESULT(x)                                                               \
    if (x != MPI_SUCCESS)                                                                          \
        throw std::runtime_error("HWMALLOC Error: MPI Call failed " + std::string(#x) + " in " +   \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));
#define HWMALLOC_CHECK_MPI_RESULT_NOEXCEPT(x)                                                      \
    if (x != MPI_SUCCESS)                                                                          \
    {                                                                                              \
        std::cerr << "HWMALLOC Error: MPI Call failed " << std::string(#x) << " in "               \
                  << std::string(__FILE__) << ":" << std::to_string(__LINE__) << std::endl;        \
        std::terminate();                                                                          \
    }
#endif
