cmake_minimum_required(VERSION 3.12)
set(name llvmPy)
project(${name})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                     -Wall -Werror -Weverything -pedantic \
                     -Wno-unused-variable -Wno-unused-parameter \
                     -Wno-c++98-compat -Wno-c++98-compat-pedantic \
                     -Wno-global-constructors -Wno-exit-time-destructors \
                     -Wno-c99-extensions \
                     -Wno-padded -Wno-switch-enum \
                     -Wno-disabled-macro-expansion")

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
        native
        OrcJIT
        RuntimeDyld)

# Libraries

add_subdirectory(library)

# llvmPy static library

add_library(
        lib${name}
        include/llvmPy/PyObj/PyBool.h
        include/llvmPy/PyObj/PyClass.h
        include/llvmPy/PyObj/PyFunc.h
        include/llvmPy/PyObj/PyInt.h
        include/llvmPy/PyObj/PyNone.h
        include/llvmPy/PyObj/PyObj.h
        include/llvmPy/PyObj/PyStr.h
        include/llvmPy/RT/Frame.h
        include/llvmPy/RT/RTModule.h
        include/llvmPy/RT/RTScope.h
        include/llvmPy/RT/Scope.h
        include/llvmPy/Support/iterator_range.h
        include/llvmPy/Support/Testing.h
        include/llvmPy/AST.h
        include/llvmPy/Compiler.h
        include/llvmPy/Emitter.h
        include/llvmPy/Instr.h
        include/llvmPy/Lexer.h
        include/llvmPy/Parser2.h
        include/llvmPy/PyObj.h
        include/llvmPy/RT.h
        include/llvmPy/Token.h
        include/llvmPy/Typed.h
        include/llvmPy.h
        src/PyObj/PyBool.cc
        src/PyObj/PyClass.cc
        src/PyObj/PyFunc.cc
        src/PyObj/PyInt.cc
        src/PyObj/PyNone.cc
        src/PyObj/PyObj.cc
        src/PyObj/PyStr.cc
        include/llvmPy/RT/Call.h
        src/RT/Frame.cc
        src/RT/RTScope.cc
        src/RT/RTModule.cc
        src/RT/Scope.cc
        src/Support/Typed.cc
        src/AST.cc
        src/Compiler.cc
        src/CompilerImpl.h
        src/CompilerImpl.cc
        src/Emitter.cc
        src/Instr.cc
        src/Lexer.cc
        src/Parser2.cc
        src/RT.cc
        src/Token.cc
)

set_target_properties(
        lib${name}
        PROPERTIES OUTPUT_NAME ${name})

target_include_directories(
        lib${name}
        PUBLIC include
        PUBLIC ${LLVM_INCLUDE_DIRS}
        PUBLIC ${Boost_INCLUDE_DIR}
        PRIVATE src)

target_compile_definitions(
        lib${name}
        PUBLIC ${LLVM_DEFINITIONS})

target_link_libraries(
        lib${name}
        PUBLIC ${llvm_libs}
        PRIVATE ${Boost_LIBRARIES})

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
        test/InstrTest.cc
        test/TestSuite.cc
        test/PyObjTest.cc
        test/Parser/Parser2Test.cc
        test/Builtins/str/builtin_str.cc
        test/Types/PyStr/type_PyStr.cc
        test/Builtins/truthy/instr_truthy.cc
        test/Builtins/bool/builtin_bool.cc
        test/Builtins/func/instr_func.cc
        test/include/catch2.h
        test/include/fakeit.h
        test/Builtins/getattr/instr_getattr.cc
        test/Builtins/fchk/instr_fchk.cc)

# The test suites depend on an up-to-date llvmPy binary.
add_dependencies(
        ${name}Test ${name})

configure_file(
        test/llvmPyTest.h.in
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
