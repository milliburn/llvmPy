set(name llvmPyLitParse)

# llvmPyLitParse

add_library(
        ${name}
        include/llvmPy/LitParser.h
        src/LitParser.cc
        include/llvmPy/Lit.h
        src/Lit.cc)

target_include_directories(
        ${name}
        PUBLIC include
        PRIVATE src)

# llvmPyLitParseTest

add_executable(
        ${name}Test
        test/main.cc
        test/LitParserTest.cc
        test/SuiteTest.cc)

configure_file(
        test/build.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/build.h)

target_include_directories(
        ${name}Test
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(
        ${name}Test
        PRIVATE ${name}
        PRIVATE Catch2)
