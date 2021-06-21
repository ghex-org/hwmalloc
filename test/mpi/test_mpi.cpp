#include <gtest/gtest.h>

//#include <hwmalloc/heap.hpp>
#include "./context.hpp"
#include <iostream>

TEST(mpi, some_test)
{

    hwmalloc::mpi::context c(MPI_COMM_WORLD);

    hwmalloc::mpi::heap h(&c);

   
    auto ptr = h.allocate(128, 0);
    std::cout << ptr.get() << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    h.free(ptr);

    MPI_Barrier(MPI_COMM_WORLD);

}

