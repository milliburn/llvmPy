set(name llvmPyLitParse)

# llvmPyLitParse

add_library(
        ${name}
        include/llvmPy/LitParser.h
        src/LitParser.cc)

target_include_directories(
        ${name}
        PUBLIC include
        PRIVATE src)

# llvmPyLitParseTest

add_executable(
        ${name}Test
        test/main.cc
        test/NonVerboseTest.cc)

target_link_libraries(
        ${name}Test
        PRIVATE ${name}
        PRIVATE Catch2)
