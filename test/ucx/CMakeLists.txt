
function(reg_test_ucx t)
    add_executable(${t} ${t}.cpp)
    target_link_libraries(${t} PRIVATE gtest_main)
    target_link_libraries(${t} PRIVATE hwmalloc)
    target_link_libraries(${t} PRIVATE Boost::boost)
    target_link_libraries(${t} PRIVATE UCP::libucp)
    add_test(NAME ${t} COMMAND $<TARGET_FILE:${t}>)
endfunction()

reg_test_ucx(test_ucx)
