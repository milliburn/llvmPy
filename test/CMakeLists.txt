set(name test)

add_executable(
        ${name}
        src/main.cc
        src/Internals/Lexer/LexerTest.cc
        src/TestSuite.cc
        src/Builtins/str/builtin_str.cc
        src/Types/PyStr/type_PyStr.cc
        src/Builtins/truthy/instr_truthy.cc
        src/Builtins/bool/builtin_bool.cc
        src/Builtins/func/instr_func.cc
        include/catch2.h
        include/fakeit.h
        src/Builtins/getattr/instr_getattr.cc
        src/Builtins/fchk/instr_fchk.cc
        src/Types/PyBool/type_PyBool.cc
        src/Types/PyNone/type_PyNone.cc
        src/Types/PyInt/type_PyInt.cc
        src/Builtins/add/builtin_add.cc
        src/Builtins/int/builtin_int.cc
        src/Builtins/none/builtin_none.cc
        src/Builtins/len/builtin_len.cc
        src/Internals/AST/AST_replace.cc
        src/Internals/AST/test_CompoundStmt.cc
        src/Internals/Parser/ParserTest.cc
        src/Types/PyTuple/type_PyTuple.cc
        include/llvmPy/Test/PyObj_DSL.h
        src/Types/PyObj/test_PyObj.cc
        src/Builtins/getitem/instr_getitem.cc
        src/Internals/Lexer/test_Lexer_utf8.cc)

# The test suites depend on an up-to-date llvmPy binary.
add_dependencies(${name} libllvmPy)

configure_file(
        src/llvmPyTest.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/llvmPyTest.h)

target_include_directories(
        ${name}
        PRIVATE include
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
        PRIVATE ${CMAKE_SOURCE_DIR}/vendor/FakeIt/single_header/catch)

target_link_libraries(
        ${name}
        PRIVATE libllvmPy
        PRIVATE llvmPyLitParse
        PRIVATE Catch2)
