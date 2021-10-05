# ---------------------------------------------------------------------
# compiler and linker flags
# ---------------------------------------------------------------------
function(hwmalloc_target_compile_options target)
    set_target_properties(${target} PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
endfunction()
