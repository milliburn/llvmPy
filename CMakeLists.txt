cmake_minimum_required(VERSION 3.12)
project(llvmPy)
set(CMAKE_CXX_STANDARD 14)
set(name llvmPy)
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
        PUBLIC include
        PRIVATE src)

