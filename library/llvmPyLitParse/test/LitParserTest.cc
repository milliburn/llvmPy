#include <catch2/catch.hpp>
#include <llvmPy/LitParser.h>
#include <sstream>
using namespace llvmPy;

TEST_CASE("NonVerbose") {
    SECTION("Parse passing result line") {
        LitParser parser("PASS: llvmPy :: Emitter/call1.py (2 of 10)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::PASS);
        CHECK(result.getSuiteName() == "llvmPy");
        CHECK(result.getTestName() == "Emitter/call1.py");
        CHECK(result.getCurrentProgress() == 2);
        CHECK(result.getMaxProgress() == 10);
    }

    SECTION("Parse failing result line") {
        LitParser parser("FAIL: llvmPy :: Emitter/Scope_Hierarchy.py (9 of 11)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::FAIL);
        CHECK(result.getSuiteName() == "llvmPy");
        CHECK(result.getTestName() == "Emitter/Scope_Hierarchy.py");
        CHECK(result.getCurrentProgress() == 9);
        CHECK(result.getMaxProgress() == 11);
    }

    SECTION("Parse unresolved result line") {
        LitParser parser("UNRESOLVED: llvmPy :: Emitter/Weird.ll (1 of 10)");
        LitTestResult &result = *parser.parseNext();
        CHECK(result.getResultCode() == LitResultCode::UNRESOLVED);
        CHECK(result.getSuiteName() == "llvmPy");
        CHECK(result.getTestName() == "Emitter/Weird.ll");
        CHECK(result.getCurrentProgress() == 1);
        CHECK(result.getMaxProgress() == 10);
    }

    SECTION("Ignore the first metadata line") {
        LitParser parser("-- Testing: 10 tests, 10 threads --");
        LitTestResult *result = parser.parseNext();
        CHECK(result == nullptr);
    }

    SECTION("Ignore the testing time metadata line") {
        LitParser parser("Testing Time: 0.12s");
        LitTestResult *result = parser.parseNext();
        CHECK(result == nullptr);
    }

    SECTION("Parse multiple successive lines") {
        std::stringstream ss;

        ss << "-- Testing: 10 tests, 10 threads --\n"
              "UNRESOLVED: llvmPy :: Emitter/Weird.ll (1 of 10)\n"
              "PASS: llvmPy :: Emitter/Literal_None.py (2 of 10)\n"
              "PASS: llvmPy :: Emitter/Empty_Module.py (3 of 10)\n"
              "PASS: llvmPy :: Emitter/Assign_Lambda.py (4 of 10)\n"
              "PASS: llvmPy :: Emitter/BinOp_Add_Ints.py (5 of 10)\n"
              "PASS: llvmPy :: Emitter/Literal_Int.py (6 of 10)\n"
              "PASS: llvmPy :: Emitter/call0.py (7 of 10)\n"
              "PASS: llvmPy :: Emitter/Assign_Int.py (8 of 10)\n"
              "PASS: llvmPy :: Emitter/call1.py (9 of 10)\n"
              "FAIL: llvmPy :: Emitter/Scope_Hierarchy.py (10 of 10)\n"
              "Testing Time: 0.12s\n"
              "********************\n"
              "Failing Tests (1):\n"
              "    llvmPy :: Emitter/Scope_Hierarchy.py\n"
              "\n"
              "********************\n"
              "Unresolved Tests (1):\n"
              "    llvmPy :: Emitter/Weird.ll\n"
              "\n"
              "  Expected Passes    : 8\n"
              "  Unresolved Tests   : 1\n"
              "  Unexpected Failures: 1\n";

        ss.seekp(std::ios_base::seekdir::beg);
        LitParser parser(ss);
        parser.parse();
        auto results = parser.getResults();

        REQUIRE(results.size() == 10);
        CHECK(results[1]->getResultCode() == LitResultCode::PASS);
        CHECK(results[9]->getResultCode() == LitResultCode::FAIL);
        CHECK(results[0]->getResultCode() == LitResultCode::UNRESOLVED);

        for (int i = 0; i < results.size(); ++i) {
            CHECK(results[i] != nullptr);
        }
    }
}
