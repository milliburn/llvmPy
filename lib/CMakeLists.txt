set(name ${LLVMPY_LIBRARY_NAME})

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

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.65 REQUIRED)

add_library(
        ${name}
        include/llvmPy/PyObj/PyBool.h
        include/llvmPy/PyObj/PyClass.h
        include/llvmPy/PyObj/PyFunc.h
        include/llvmPy/PyObj/PyInt.h
        include/llvmPy/PyObj/PyNone.h
        include/llvmPy/PyObj/PyObj.h
        include/llvmPy/PyObj/PyStr.h
        include/llvmPy/PyObj/PyTuple.h
        include/llvmPy/RT/Frame.h
        include/llvmPy/RT/RTModule.h
        include/llvmPy/RT/RTScope.h
        include/llvmPy/RT/Scope.h
        include/llvmPy/Support/iterator_range.h
        include/llvmPy/Support/Testing.h
        include/llvmPy/AST.h
        include/llvmPy/Compiler.h
        include/llvmPy/Compiler/AllPhases.h
        include/llvmPy/Compiler/Emitter/EmitterPhase.h
        include/llvmPy/Compiler/Lexer/LexerPhase.h
        include/llvmPy/Compiler/Parser/AstPass.h
        include/llvmPy/Compiler/Parser/DelambdafyRule.h
        include/llvmPy/Compiler/Parser/ParserPhase.h
        include/llvmPy/Compiler/Phase.h
        include/llvmPy/Compiler/Runtime/RuntimePhase.h
        include/llvmPy/Emitter.h
        include/llvmPy/Instr.h
        include/llvmPy/Lexer.h
        include/llvmPy/PyObj.h
        include/llvmPy/Python.h
        include/llvmPy/RT.h
        include/llvmPy/RT/Call.h
        include/llvmPy/Token.h
        include/llvmPy/Typed.h
        include/llvmPy.h
        src/AST.cc
        src/Compiler.cc
        src/Compiler/Emitter/EmitterPhase.cc
        src/Compiler/Lexer/LexerPhase.cc
        src/Compiler/Parser/AstPass.cc
        src/Compiler/Parser/DelambdafyRule.cc
        src/Compiler/Parser/ParserPhase.cc
        src/Compiler/Runtime/RuntimePhase.cc
        src/CompilerImpl.cc
        src/CompilerImpl.h
        src/Emitter.cc
        src/Instr.cc
        src/Lexer.cc
        src/Parser.cc
        src/PyObj/PyBool.cc
        src/PyObj/PyClass.cc
        src/PyObj/PyFunc.cc
        src/PyObj/PyInt.cc
        src/PyObj/PyNone.cc
        src/PyObj/PyObj.cc
        src/PyObj/PyStr.cc
        src/PyObj/PyTuple.cc
        src/Python.cc
        src/PythonImpl.cc
        src/PythonImpl.h
        src/RT.cc
        src/RT/Frame.cc
        src/RT/RTScope.cc
        src/RT/RTModule.cc
        src/RT/Scope.cc
        src/Support/Typed.cc
        src/Token.cc)

set_target_properties(
        ${name}
        PROPERTIES OUTPUT_NAME ${name})

target_include_directories(
        ${name}
        PUBLIC include
        PUBLIC ${LLVM_INCLUDE_DIRS}
        PUBLIC ${Boost_INCLUDE_DIR}
        PRIVATE src)

target_compile_definitions(
        ${name}
        PUBLIC ${LLVM_DEFINITIONS})

target_link_libraries(
        ${name}
        PUBLIC ${llvm_libs}
        PRIVATE ${Boost_LIBRARIES})
