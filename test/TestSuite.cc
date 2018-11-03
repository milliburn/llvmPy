#include <llvmPy/Lit.h>
#include <llvmPy/LitParser.h>
#include <catch2/catch.hpp>
#include <llvmPyTest.h>
#include <stdlib.h>
using namespace llvmPy;

TEST_CASE("Test Suite") {
    std::string PATH(getenv("PATH"));
    PATH = std::string(BINARY_DIR) +
            ":" +
            std::string(LLVM_DIR) +
            "/../../../bin:" +
            PATH;
    setenv("PATH", PATH.c_str(), 1);
    std::vector<LitTestResult *> results;
    lit(results, TEST_PATH);

    for (auto result : results) {
        SECTION(result->getTestName()) {
            switch (result->getResultCode()) {
            case LitResultCode::PASS:
            case LitResultCode::XPASS:
                SUCCEED(
                        "Test case successful (" <<
                        result->getResultCode() <<
                        ").");
                break;

            default:
                FAIL_CHECK(
                        "Test case " <<
                         result->getSuiteName() <<
                         " :: " <<
                         result->getTestName() <<
                         " failed with status " <<
                         result->getResultCode() <<
                         " and the following output:\n\n" <<
                         result->getOutput());
                break;
            }
        }
    }
}
