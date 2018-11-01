#include <catch2/catch.hpp>
#include <llvmPy/LitParser.h>
using namespace llvmPy;

TEST_CASE("NonVerbose") {
    SECTION("Parse passing result line") {
        LitParser parser("PASS: llvmPy :: Emitter/call1.py (2 of 10)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::PASS);
    }
}
