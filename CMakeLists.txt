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

add_test(NAME prints-help
    COMMAND fmtdir --help)
set_tests_properties(prints-help PROPERTIES
    PASS_REGULAR_EXPRESSION "Usage:"
)

add_test(NAME help-exits-successfully
    COMMAND fmtdir -h)

add_test(NAME fails-on-two-directories
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(fails-on-two-directories PROPERTIES WILL_FAIL TRUE)

# .gitignore is hidden on Unix-like systems; with -a it is listed everywhere
add_test(NAME all-option-before-directory
    COMMAND fmtdir -a ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(all-option-before-directory PROPERTIES
    PASS_REGULAR_EXPRESSION "\\.gitignore"
)

add_test(NAME all-option-after-directory
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR} --all)
set_tests_properties(all-option-after-directory PROPERTIES
    PASS_REGULAR_EXPRESSION "\\.gitignore"
)

add_test(NAME hidden-option-still-works
    COMMAND fmtdir --hidden ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(hidden-option-still-works PROPERTIES
    PASS_REGULAR_EXPRESSION "\\.gitignore"
)

# After "--", "-a" is a (missing) directory: the error names it, and no
# usage message is printed
add_test(NAME double-dash-ends-options
    COMMAND fmtdir -- -a)
set_tests_properties(double-dash-ends-options PROPERTIES
    PASS_REGULAR_EXPRESSION "-a"
    FAIL_REGULAR_EXPRESSION "Usage:"
)

# LICENSE has no extension
add_test(NAME labels-files-without-extension
    COMMAND fmtdir ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(labels-files-without-extension PROPERTIES
    PASS_REGULAR_EXPRESSION "\\(no extension\\):"
)
