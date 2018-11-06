#include <llvmPy/Lit.h>
#include <llvmPy/LitParser.h>
#include <catch2/catch.hpp>
#include <llvmPyTest.h>
#include <stdlib.h>
#include <sstream>
using namespace llvmPy;

static std::vector<std::string> tokenize(
        std::string const &string,
        char const delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream iss(string);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

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
        std::vector<std::string> sections =
                tokenize(result->getTestName(), '/');

        REQUIRE(sections.size() == 2);

        SECTION(sections[0]) {
            SECTION(sections[1]) {
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
}
