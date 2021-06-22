/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "mpi_setup.hpp"

GTEST_API_ int
main(int argc, char** argv)
{
    init_mpi(argc, argv);

    auto result = run_tests();

    finalize_mpi();

    return result;
}
