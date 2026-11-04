cmake_minimum_required(VERSION 3.10)

project(fmtdir CXX)

set (CMAKE_CXX_STANDARD 17)
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
endif()

add_executable(fmtdir fmtdir.cc)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_target_properties(fmtdir PROPERTIES
        OSX_ARCHITECTURES "x86_64;arm64"
    )
endif()

enable_testing()

add_test(NAME runs-on-directory
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME lists-files
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(lists-files PROPERTIES
    PASS_REGULAR_EXPRESSION "CMakeLists\\.txt"
)

add_test(NAME fails-on-missing-directory
    COMMAND fmtdir ${CMAKE_CURRENT_BINARY_DIR}/does-not-exist)
set_tests_properties(fails-on-missing-directory PROPERTIES WILL_FAIL TRUE)

add_test(NAME fails-on-unknown-option
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR} --bogus)
set_tests_properties(fails-on-unknown-option PROPERTIES WILL_FAIL TRUE)

add_test(NAME fails-without-arguments
    COMMAND fmtdir)
set_tests_properties(fails-without-arguments PROPERTIES WILL_FAIL TRUE)

add_test(NAME prints-usage-without-arguments
    COMMAND fmtdir)
set_tests_properties(prints-usage-without-arguments PROPERTIES
    PASS_REGULAR_EXPRESSION "Usage:"
)
