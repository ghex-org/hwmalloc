#include <gtest/gtest.h>
#include "./mpi_test_fixture.hpp"

#include <hwmalloc/heap.hpp>
#include <hwmalloc/mpi/context.hpp>
#include <iostream>

TEST_F(mpi_test_fixture, some_test)
{
    hwmalloc::mpi::context c(MPI_COMM_WORLD);

    hwmalloc::heap<hwmalloc::mpi::context> h(&c);

    auto ptr = h.allocate(8, 0);

    std::cout << ptr.handle().get_remote_key() << std::endl;

    std::cout << ptr.get() << std::endl;

    h.free(ptr);
}
