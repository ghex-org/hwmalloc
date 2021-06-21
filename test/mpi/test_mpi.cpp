#include <gtest/gtest.h>

//#include <hwmalloc/heap.hpp>
#include <hwmalloc/mpi/context.hpp>
#include <iostream>

TEST(mpi, some_test)
{

    hwmalloc::mpi::context c(MPI_COMM_WORLD);

    hwmalloc::mpi::heap h(&c);

   
    MPI_Barrier(MPI_COMM_WORLD);
    auto ptr = h.allocate(8, 0);
    std::cout << ptr.get() << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    h.free(ptr);

    MPI_Barrier(MPI_COMM_WORLD);

}

