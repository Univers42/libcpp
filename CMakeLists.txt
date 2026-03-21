cmake_minimum_required(VERSION 3.14)
project(libcpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

find_package(Threads REQUIRED)

# Collect all source files from src/
file(GLOB_RECURSE LIBCPP_SOURCES "src/*.cpp")

# Build as static library
add_library(libcpp STATIC ${LIBCPP_SOURCES})
target_include_directories(libcpp PUBLIC include)
target_link_libraries(libcpp PUBLIC Threads::Threads)

# Alias target: libftpp
add_library(libftpp ALIAS libcpp)

# Test executable
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(libcpp_tests ${TEST_SOURCES})
    target_link_libraries(libcpp_tests PRIVATE libcpp)
    target_include_directories(libcpp_tests PRIVATE include)
endif()

# Demo executable
if(EXISTS "${CMAKE_SOURCE_DIR}/demo/demo_libftpp.cpp")
    add_executable(demo_libftpp demo/demo_libftpp.cpp)
    target_link_libraries(demo_libftpp PRIVATE libcpp)
    target_include_directories(demo_libftpp PRIVATE include ${CMAKE_SOURCE_DIR})
endif()
