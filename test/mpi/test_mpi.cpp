#include <gtest/gtest.h>
#include "./mpi_test_fixture.hpp"

#include <hwmalloc/heap.hpp>
#include <hwmalloc/mpi/context.hpp>
#include <iostream>
#include <vector>

TEST_F(mpi_test_fixture, allocate)
{
    hwmalloc::mpi::context c(MPI_COMM_WORLD);

    hwmalloc::heap<hwmalloc::mpi::context> h(&c);

    auto ptr = h.allocate(8, 0);

    std::cout << ptr.handle().get_remote_key() << std::endl;

    std::cout << ptr.get() << std::endl;

    h.free(ptr);
}

TEST_F(mpi_test_fixture, sendrecv)
{
    using context_type = hwmalloc::mpi::context;
    using heap_type = hwmalloc::heap<context_type>;
    using value_type = int;
    using allocator_type = typename heap_type::template allocator_type<value_type>;
    using vector_type = std::vector<value_type, allocator_type>;

    context_type c(MPI_COMM_WORLD);
    heap_type    h(&c);

    vector_type send_buffer(16, world_rank, h.get_allocator<value_type>(0));
    vector_type recv_buffer(16, -1, h.get_allocator<value_type>(0));

    int send_rank = (world_rank + 1) % world_size;
    int recv_rank = (world_rank + world_size - 1) % world_size;

    HWMALLOC_CHECK_MPI_RESULT(MPI_Sendrecv(send_buffer.data(), // send buffer
        send_buffer.size() * sizeof(value_type),               // send count
        MPI_BYTE,                                              // MPI datatype
        send_rank,                                             // destination
        world_rank,                                            // send tag
        recv_buffer.data(),                                    // recv buffer
        recv_buffer.size() * sizeof(value_type),               // recv count
        MPI_BYTE,                                              // MPI datatype
        recv_rank,                                             // source
        recv_rank,                                             // recv tag
        MPI_COMM_WORLD,                                        // communicator
        MPI_STATUS_IGNORE));

    // test if recieved buffer is correct
    for (auto x : recv_buffer) { EXPECT_EQ(x, recv_rank); }
}

TEST_F(mpi_test_fixture, put)
{
    using context_type = hwmalloc::mpi::context;
    using heap_type = hwmalloc::heap<context_type>;
    using value_type = int;

    context_type c(MPI_COMM_WORLD);
    heap_type    h(&c);

    constexpr std::size_t size = 10;

    auto send_buffer = h.make_unique<value_type[]>(0, size);
    auto recv_buffer = h.make_unique<value_type[]>(0, size);

    for (std::size_t i = 0; i < size; ++i)
    {
        send_buffer[i] = world_rank;
        recv_buffer[i] = -1;
    }

    int send_rank = (world_rank + 1) % world_size;
    int recv_rank = (world_rank + world_size - 1) % world_size;

    // exchange handle
    auto my_key = recv_buffer.get().handle().get_remote_key();
    using key_type = decltype(my_key);
    key_type remote_key;
    HWMALLOC_CHECK_MPI_RESULT(MPI_Sendrecv(&my_key, // send buffer
        sizeof(key_type),                           // send count
        MPI_BYTE,                                   // MPI datatype
        recv_rank,                                  // destination
        world_rank,                                 // send tag
        &remote_key,                                // recv buffer
        sizeof(key_type),                           // recv count
        MPI_BYTE,                                   // MPI datatype
        send_rank,                                  // source
        send_rank,                                  // recv tag
        MPI_COMM_WORLD,                             // communicator
        MPI_STATUS_IGNORE));

    // put
    auto addr = static_cast<void const*>(send_buffer.get());
    HWMALLOC_CHECK_MPI_RESULT(MPI_Put(addr, // origin address
        size * sizeof(value_type),          // origin count
        MPI_BYTE,                           // origin MPI datatype
        send_rank,                          // target rank
        remote_key,                         // target displacement
        size * sizeof(value_type),          // target count
        MPI_BYTE,                           // target MPI datatype
        c.get_window()));

    HWMALLOC_CHECK_MPI_RESULT(MPI_Win_fence(0, c.get_window()));

    // test if recieved buffer is correct
    for (std::size_t i = 0; i < size; ++i) EXPECT_EQ(recv_buffer[i], recv_rank);
}
