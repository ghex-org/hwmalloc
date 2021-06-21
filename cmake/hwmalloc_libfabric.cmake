# set all libfabric related options and values

#------------------------------------------------------------------------------
# Enable libfabric support
#------------------------------------------------------------------------------
hwmalloc_option(HWMALLOC_WITH_LIBFABRIC BOOL
  "Enable libfabric memory registration/pinning module"
  OFF CATEGORY "hwmalloc") # ADVANCED)

if (HWMALLOC_WITH_LIBFABRIC)
  find_package(Libfabric REQUIRED)
  hwmalloc_add_config_define_namespace(
      DEFINE    HWMALLOC_HAVE_LIBFABRIC
      NAMESPACE libfabric)
else()
  return()
endif()

#------------------------------------------------------------------------------
# Hardware device selection
#------------------------------------------------------------------------------
hwmalloc_option(HWMALLOC_LIBFABRIC_PROVIDER STRING
  "The provider (verbs/gni/psm2/tcp/sockets)"
  "verbs" CATEGORY "hwmalloc" ADVANCED)

hwmalloc_add_config_define_namespace(
    DEFINE HWMALLOC_LIBFABRIC_PROVIDER
    VALUE  "\"${HWMALLOC_LIBFABRIC_PROVIDER}\""
    NAMESPACE libfabric)

if(HWMALLOC_LIBFABRIC_PROVIDER MATCHES "verbs")
    hwmalloc_add_config_define_namespace(
        DEFINE HWMALLOC_LIBFABRIC_VERBS
        NAMESPACE libfabric)
elseif(HWMALLOC_LIBFABRIC_PROVIDER MATCHES "gni")
    hwmalloc_add_config_define_namespace(
        DEFINE HWMALLOC_LIBFABRIC_GNI
        NAMESPACE libfabric)
elseif(HWMALLOC_LIBFABRIC_PROVIDER MATCHES "tcp")
    hwmalloc_add_config_define_namespace(
        DEFINE HWMALLOC_LIBFABRIC_TCP
        NAMESPACE libfabric)
elseif(HWMALLOC_LIBFABRIC_PROVIDER MATCHES "sockets")
    hwmalloc_add_config_define_namespace(
        DEFINE HWMALLOC_LIBFABRIC_SOCKETS
        NAMESPACE libfabric)
elseif(HWMALLOC_LIBFABRIC_PROVIDER MATCHES "psm2")
    hwmalloc_add_config_define_namespace(
        DEFINE HWMALLOC_LIBFABRIC_PSM2
        NAMESPACE libfabric)
endif()

#------------------------------------------------------------------------------
# Write options to file in build dir
#------------------------------------------------------------------------------
write_config_defines_file(
    NAMESPACE libfabric
    FILENAME  "${PROJECT_BINARY_DIR}/include/hwmalloc/libfabric/config.hpp"
)

target_include_directories(libfabric::libfabric INTERFACE "${PROJECT_BINARY_DIR}")
