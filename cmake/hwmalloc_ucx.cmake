# set all ucx related options and values

#------------------------------------------------------------------------------
# Enable ucx support
#------------------------------------------------------------------------------
hwmalloc_option(HWMALLOC_WITH_UCX BOOL
  "Enable ucx memory registration/pinning module"
  OFF CATEGORY "hwmalloc")

if (HWMALLOC_WITH_UCX)
    find_package(UCP REQUIRED)
    hwmalloc_add_config_define_namespace(
        DEFINE    HWMALLOC_HAVE_UCX
        NAMESPACE ucx)
else()
    return()
endif()

#------------------------------------------------------------------------------
# Write options to file in build dir
#------------------------------------------------------------------------------
write_config_defines_file(
    NAMESPACE ucx
    FILENAME  "${PROJECT_BINARY_DIR}/include/hwmalloc/ucx/config.hpp"
)
