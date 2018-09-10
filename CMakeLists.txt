cmake_minimum_required(VERSION 3.12)
project(llvmPy)
set(name llvmPy)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
set(CMAKE_CXX_STANDARD 14)

add_executable(
        ${name}
        src/main.cc
        include/llvmPy/AST/Token.h
        src/AST/Token.cc
        include/llvmPy/AST/Expr.h
        src/AST/Expr.cc
        include/llvmPy/AST.h)

target_include_directories(
        ${name}
        PUBLIC
            include
        PRIVATE
            src
            ${LLVM_INCLUDE_DIRS})

target_compile_definitions(
        ${name} PRIVATE ${LLVM_DEFINITIONS})
