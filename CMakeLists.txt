cmake_minimum_required(VERSION 3.0)
enable_language(CXX)

project(brim)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
enable_testing()

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

add_subdirectory(src)
add_subdirectory(test)
