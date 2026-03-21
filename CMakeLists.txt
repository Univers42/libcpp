cmake_minimum_required(VERSION 2.8)
project(libcpp)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Collect all source files from src/
file(GLOB_RECURSE LIBCPP_SOURCES "src/*.cpp")

# Build as static library
add_library(libcpp STATIC ${LIBCPP_SOURCES})
target_include_directories(libcpp PUBLIC include)

# Test executable
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(libcpp_tests ${TEST_SOURCES})
    target_link_libraries(libcpp_tests libcpp)
    target_include_directories(libcpp_tests PRIVATE include)
endif()
