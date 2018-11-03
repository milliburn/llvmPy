#include <llvmPy/Lit.h>
#include <llvmPy/LitParser.h>
#include <catch2/catch.hpp>
#include <llvmPyTest.h>
using namespace llvmPy;

TEST_CASE("Test Suite") {
    std::vector<LitTestResult *> results;
    lit(results, TEST_PATH);

    for (auto result : results) {
        DYNAMIC_SECTION(result->getTestName()) {
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
