#include <llvmPy/Lit.h>
#include <llvmPy/LitParser.h>
#include <catch2/catch.hpp>
#include <llvmPyTest.h>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
using namespace llvmPy;
using std::map;
using std::string;
using std::vector;

struct Section {
    map<string, std::unique_ptr<Section>> subsections;
    vector<LitTestResult *> results;
};

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

static std::vector<LitTestResult *>
runTests()
{
    std::string PATH(getenv("PATH"));
    PATH = std::string(BINARY_DIR) +
           ":" +
           std::string(LLVM_DIR) +
           "/../../../bin:" +
           PATH;
    setenv("PATH", PATH.c_str(), 1);
    vector<LitTestResult *> results;
    lit(results, TEST_PATH);
    return results;
}

static std::unique_ptr<Section>
buildResults(vector<LitTestResult *> const &results)
{
    auto output = std::make_unique<Section>();

    for (auto result : results) {
        vector<string> sections = tokenize(result->getTestName(), '/');
        Section *next = output.get();

        for (auto section : sections) {
            if (!next->subsections.count(section)) {
                next->subsections[section] = std::make_unique<Section>();
            }

            next = next->subsections[section].get();
        }

        next->results.push_back(result);
    }

    return output;
}

static void
report(LitTestResult const &result)
{
    switch (result.getResultCode()) {
    case LitResultCode::PASS:
    case LitResultCode::XPASS:
        SUCCEED(
                "Test case successful (" <<
                result.getResultCode() <<
                ").");
        break;

    default:
        FAIL_CHECK(
                "Test case " <<
                result.getSuiteName() <<
                " :: " <<
                result.getTestName() <<
                " failed with status " <<
                result.getResultCode() <<
                " and the following output:\n\n" <<
                result.getOutput());
        break;
    }
}

static void
tree(Section const &section)
{
    for (auto result : section.results) {
        report(*result);
    }

    for (auto &subsection : section.subsections) {
        SECTION(subsection.first) {
            tree(*subsection.second);
        }
    }
}

static std::unique_ptr<Section> testResults;

TEST_CASE("Test Suite") {
    if (!testResults) {
        auto results = runTests();
        testResults = buildResults(results);
    }

    tree(*testResults);
}
