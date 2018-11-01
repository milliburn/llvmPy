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
            std::string const &suiteName,
            std::string const &testName,
            std::string const &output,
            int currentProgress,
            int maxProgress);

    LitResultCode getResultCode() const;
    std::string getSuiteName() const;
    std::string getTestName() const;
    std::string getOutput() const;
    int getCurrentProgress() const;
    int getMaxProgress() const;

private:
    LitResultCode resultCode;
    std::string suiteName;
    std::string testName;
    std::string output;
    int const currentProgress;
    int const maxProgress;
};

/**
 * Recursive descent parser implementing the lit (LLVM testing tool)
 * output format described at: https://llvm.org/docs/CommandGuide/lit.html#test-run-output-format
 */
class LitParser {
public:
    explicit LitParser(std::istream const &stream);
    explicit LitParser(std::string const &input);
    LitTestResult *parseNext();
    void parse();
    std::vector<LitTestResult *> getResults() const;

private:
    std::istream &stream;
    std::vector<LitTestResult *> results;
    char ch;

    char get();
    void next();
    bool is(char const *any);
    void expect(std::string const &str);
    bool isLogDelineator();
    bool isTrailingDelineator();
    bool isResultCode(LitResultCode *resultCode = nullptr);
    bool isTestName(std::string *testName = nullptr);
    bool isNumber(int *number);
};

} // namespace
#endif // __cplusplus
