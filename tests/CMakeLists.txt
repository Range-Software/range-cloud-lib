# Unit tests for range-cloud-lib (QTest based).
#
# Enabled by configuring the project with -DRANGE_BUILD_TESTS=ON.
# Run with `ctest` from the build directory.

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)

set(range_cloud_lib_tests
    tst_access_mode
    tst_access_owner
    tst_file_quota
    tst_auth_token
)

foreach(test_name IN LISTS range_cloud_lib_tests)
    qt_add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name}
        PRIVATE
            range-cloud-lib
            Qt6::Test
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
