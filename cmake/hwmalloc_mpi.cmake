# set all mpi related options and values

#------------------------------------------------------------------------------
# Enable mpi support
#------------------------------------------------------------------------------
hwmalloc_option(HWMALLOC_WITH_MPI BOOL
  "Enable mpi memory registration/pinning module"
  OFF CATEGORY "hwmalloc")

if (HWMALLOC_WITH_MPI)
    find_package(MPI REQUIRED COMPONENTS CXX)
    hwmalloc_add_config_define_namespace(
        DEFINE    HWMALLOC_HAVE_MPI
        NAMESPACE mpi)
else()
  return()
endif()

#------------------------------------------------------------------------------
# Write options to file in build dir
#------------------------------------------------------------------------------
write_config_defines_file(
    NAMESPACE mpi
    FILENAME  "${PROJECT_BINARY_DIR}/include/hwmalloc/mpi/config.hpp"
)
