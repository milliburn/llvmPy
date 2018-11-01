#pragma once

#include <istream>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

enum class LitResultCode {
    /// The test succeeded.
    PASS,
    /// The test failed, but it's expected.
    XFAIL,
    /// The test passed, but was expected to fail.
    XPASS,
    /// The test failed.
    FAIL,
    /// The test result could not be determined (e.g. no RUN: lines).
    UNRESOLVED,
    /// Test not supported in this environment.
    UNSUPPORTED,
};

class LitTestResult {
public:
    LitTestResult(
            LitResultCode resultCode,
            std::string const &testName,
            std::string const &output);

    LitResultCode getResultCode() const;
    std::string getTestName() const;
    std::string getOutput() const;

private:
    LitResultCode resultCode;
    std::string testName;
    std::string output;
};

/**
 * Recursive descent parser implementing the lit (LLVM testing tool)
 * output format described at: https://llvm.org/docs/CommandGuide/lit.html#test-run-output-format
 */
class LitParser {
public:
    explicit LitParser(std::istream const &stream);
    explicit LitParser(std::string const &string);
    LitTestResult *parseNext();
    void parse();
    std::vector<LitTestResult *> getResults() const;

private:
    std::istream const &stream;
    std::vector<LitTestResult *> results;

    void next();
    bool is(char const *any);
    bool isLogDelineator();
    bool isTrailingDelineator();
    bool isResultCode(LitResultCode *resultCode = nullptr);
    bool isTestName(std::string *testName = nullptr);
};

} // namespace
#endif // __cplusplus
