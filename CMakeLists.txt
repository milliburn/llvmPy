cmake_minimum_required(VERSION 3.10)
set(name llvmPy)
project(${name})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

# VERSIONING

find_package(Git QUIET REQUIRED)

set(project_name "${name}")

execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --always HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE project_version
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND "${GIT_EXECUTABLE}" show -s --format=%cd
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE project_timestamp
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND
             PROPERTY CMAKE_CONFIGURE_DEPENDS
             "${CMAKE_SOURCE_DIR}/.git/index")

# Libraries

add_subdirectory(lib)
add_subdirectory(library)
add_subdirectory(vendor)

# llvmPy executable

add_subdirectory(cli)

# llvmPyTest

add_executable(
        ${name}Test
        test/src/main.cc
        test/src/Internals/Lexer/LexerTest.cc
        test/src/TestSuite.cc
        test/src/Builtins/str/builtin_str.cc
        test/src/Types/PyStr/type_PyStr.cc
        test/src/Builtins/truthy/instr_truthy.cc
        test/src/Builtins/bool/builtin_bool.cc
        test/src/Builtins/func/instr_func.cc
        test/include/catch2.h
        test/include/fakeit.h
        test/src/Builtins/getattr/instr_getattr.cc
        test/src/Builtins/fchk/instr_fchk.cc
        test/src/Types/PyBool/type_PyBool.cc
        test/src/Types/PyNone/type_PyNone.cc
        test/src/Types/PyInt/type_PyInt.cc
        test/src/Builtins/add/builtin_add.cc
        test/src/Builtins/int/builtin_int.cc
        test/src/Builtins/none/builtin_none.cc
        test/src/Builtins/len/builtin_len.cc
        test/src/Internals/AST/AST_replace.cc
        test/src/Internals/AST/test_CompoundStmt.cc
        test/src/Internals/Parser/ParserTest.cc
        test/src/Types/PyTuple/type_PyTuple.cc
        test/include/llvmPy/Test/PyObj_DSL.h
        test/src/Types/PyObj/test_PyObj.cc
        test/src/Builtins/getitem/instr_getitem.cc)

# The test suites depend on an up-to-date llvmPy binary.
add_dependencies(
        ${name}Test ${name})

configure_file(
        test/src/llvmPyTest.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/llvmPyTest.h)

target_include_directories(
        ${name}Test
        PRIVATE test/include
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
        PRIVATE library/FakeIt/single_header/catch)

target_link_libraries(
        ${name}Test
        PRIVATE lib${name}
        PRIVATE llvmPyLitParse
        PRIVATE Catch2)
