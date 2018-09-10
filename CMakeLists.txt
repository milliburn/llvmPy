cmake_minimum_required(VERSION 3.12)
set(name llvmPy)
project(${name})

set(CMAKE_CXX_STANDARD 14)

# llvmPy

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_library(
        ${name}
        include/llvmPy/AST/Token.h
        src/AST/Token.cc
        include/llvmPy/AST/Expr.h
        src/AST/Expr.cc
        include/llvmPy/AST.h
        include/llvmPy/Codegen.h
        src/Codegen.cc)

target_include_directories(
        ${name}
        PUBLIC
            include
            ${LLVM_INCLUDE_DIRS}
        PRIVATE
            src)

target_compile_definitions(
        ${name}
        PUBLIC
            ${LLVM_DEFINITIONS})

target_link_libraries(
        ${name}
        PUBLIC
            ${llvm_libs})

# llvmPyRunner

add_executable(
        ${name}Runner
        runner/main.cc)

target_link_libraries(
        ${name}Runner
        PRIVATE
            ${name})

# llvmPyTest

add_subdirectory(library/Catch2)

add_executable(
        ${name}Test
        test/main.cc)

target_link_libraries(
        ${name}Test
        PRIVATE
            ${name}
            Catch2)
