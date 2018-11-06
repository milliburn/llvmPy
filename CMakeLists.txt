cmake_minimum_required(VERSION 3.12)
set(name llvmPy)
project(${name})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# LLVM

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
llvm_map_components_to_libnames(
        llvm_libs
        support
        core
        executionengine
        irreader
        native)

# Libraries

add_subdirectory(library)

# llvmPy static library

add_library(
        lib${name}
        include/llvmPy/AST.h
        include/llvmPy/Compiler.h
        include/llvmPy/Emitter.h
        include/llvmPy/Instr.h
        include/llvmPy/Lexer.h
        include/llvmPy/Parser.h
        include/llvmPy/RT.h
        include/llvmPy/RT/Frame.h
        include/llvmPy/SyntaxError.h
        include/llvmPy/Token.h
        include/llvmPy/Typed.h
        include/llvmPy.h
        src/AST.cc
        src/Compiler.cc
        src/Emitter.cc
        src/Instr.cc
        src/Lexer.cc
        src/Parser.cc
        src/RT.cc
        src/SyntaxError.cc
        src/Token.cc
        include/llvmPy/PyObj.h
        src/PyObj.cc)

set_target_properties(
        lib${name}
        PROPERTIES OUTPUT_NAME ${name})

target_include_directories(
        lib${name}
        PUBLIC include
        PUBLIC ${LLVM_INCLUDE_DIRS}
        PRIVATE src)

target_compile_definitions(
        lib${name}
        PUBLIC ${LLVM_DEFINITIONS})

target_link_libraries(
        lib${name}
        PUBLIC ${llvm_libs})

# llvmPy executable

add_executable(
        ${name}
        src/main.cc)

target_link_libraries(
        ${name}
        PRIVATE lib${name})

# llvmPyTest

add_executable(
        ${name}Test
        test/main.cc
        test/LexerTest.cc
        test/ParserTest.cc
        test/InstrTest.cc
        test/TestSuite.cc)

# The test suites depend on an up-to-date llvmPy binary.
add_dependencies(
        ${name}Test ${name})

configure_file(
        test/llvmPyTest.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/llvmPyTest.h)

target_include_directories(
        ${name}Test
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(
        ${name}Test
        PRIVATE lib${name}
        PRIVATE llvmPyLitParse
        PRIVATE Catch2)
