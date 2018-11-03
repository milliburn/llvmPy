#include <catch2/catch.hpp>
#include <llvmPy/LitParser.h>
#include <sstream>
using namespace llvmPy;

TEST_CASE("Unit tests") {
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

    SECTION("Parse test output") {
        std::string testOutput =
              "Script:\n"
              "--\n"
              ": 'RUN: at line 1';   llvmPy --ir /Users/roberth/tetratom/llvmPy/test/Emitter/Scope_Hierarchy.py > /private/var/folder\n"
              "s/f3/gjd1d_b53_gf6cd9l5zqv4c00000gp/T/tmpeu7i1u7kllvmPy-lit-/Emitter/Output/Scope_Hierarchy.py.tmp1\n"
              ": 'RUN: at line 2';   cat -n /private/var/folders/f3/gjd1d_b53_gf6cd9l5zqv4c00000gp/T/tmpeu7i1u7kllvmPy-lit-/Emitter/O\n"
              "utput/Scope_Hierarchy.py.tmp1 >&2\n"
              ": 'RUN: at line 3';   llvm-as < /private/var/folders/f3/gjd1d_b53_gf6cd9l5zqv4c00000gp/T/tmpeu7i1u7kllvmPy-lit-/Emitte\n"
              "r/Output/Scope_Hierarchy.py.tmp1 | llvm-dis | FileCheck /Users/roberth/tetratom/llvmPy/test/Emitter/Scope_Hierarchy.py\n"
              "--\n"
              "Exit Code: 1\n"
              "\n"
              "Command Output (stderr):\n"
              // ...snip...
              "<stdin>:40:2: note: with variable \"RV\" equal to \"%7\"\n"
              " ret %PyObj* %7\n"
              " ^\n"
              "\n"
              "--\n"
              "\n";

        std::stringstream ss;
        ss << "FAIL: llvmPy :: Emitter/Scope_Hierarchy.py (5 of 10)\n"
              "******************** TEST 'llvmPy :: Emitter/Scope_Hierarchy.py' FAILED ********************\n";
        ss << testOutput;
        ss << "********************\n";

        ss.seekp(std::ios_base::seekdir::beg);
        LitParser parser(ss);
        LitTestResult *result = parser.parseNext();
        CHECK(result->getResultCode() == LitResultCode::FAIL);
        CHECK(result->getSuiteName() == "llvmPy");
        CHECK(result->getTestName() == "Emitter/Scope_Hierarchy.py");
        CHECK(result->getCurrentProgress() == 5);
        CHECK(result->getMaxProgress() == 10);
        CHECK(result->getOutput() == testOutput);
    }
}
