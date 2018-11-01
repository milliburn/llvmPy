#include <catch2/catch.hpp>
#include <llvmPy/LitParser.h>
using namespace llvmPy;

TEST_CASE("NonVerbose") {
    SECTION("Parse passing result line") {
        LitParser parser("PASS: llvmPy :: Emitter/call1.py (2 of 10)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::PASS);
        CHECK(result.getSuiteName() == "llvmPy");
        CHECK(result.getTestName() == "Emitter/call1.py");
    }

    SECTION("Parse failing result line") {
        LitParser parser("FAIL: llvmPy :: Emitter/Scope_Hierarchy.py (9 of 10)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::FAIL);
        CHECK(result.getSuiteName() == "llvmPy");
        CHECK(result.getTestName() == "Emitter/Scope_Hierarchy.py");
    }
}
