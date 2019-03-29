cmake_minimum_required(VERSION 3.10)

project(llvmPy)
set(LLVMPY_LIBRARY_NAME lib${PROJECT_NAME})
set(LLVMPY_EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

add_subdirectory(misc)
add_subdirectory(lib)
add_subdirectory(vendor)
add_subdirectory(cli)
add_subdirectory(test)
