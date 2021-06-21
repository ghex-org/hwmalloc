find_package(MPI REQUIRED COMPONENTS CXX)
target_sources(hwmalloc PRIVATE context.cpp)
target_link_libraries(hwmalloc PRIVATE MPI::MPI_CXX)
