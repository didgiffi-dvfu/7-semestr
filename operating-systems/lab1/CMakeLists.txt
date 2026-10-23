cmake_minimum_required(VERSION 3.16)

project(hello_world VERSION 1.0 LANGUAGES C)

add_executable(hello
    src/main.c
    src/greeting.c
)

target_include_directories(hello PRIVATE include)
target_compile_features(hello PRIVATE c_std_11)

if(MSVC)
    target_compile_options(hello PRIVATE /W4)
else()
    target_compile_options(hello PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME hello_output COMMAND hello)
set_tests_properties(hello_output PROPERTIES
    PASS_REGULAR_EXPRESSION "Hello, World!"
)

