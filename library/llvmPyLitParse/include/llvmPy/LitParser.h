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

std::ostream & operator<< (std::ostream &, llvmPy::LitResultCode const &);

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
    LitResultCode _resultCode;
    std::string _suiteName;
    std::string _testName;
    std::string _output;
    int const _currentProgress;
    int const _maxProgress;
};

/**
 * Recursive descent parser implementing the lit (LLVM testing tool)
 * output format described at: https://llvm.org/docs/CommandGuide/lit.html#test-run-output-format
 */
class LitParser {
public:
    explicit LitParser(std::istream &stream);
    explicit LitParser(std::string const &input);
    LitTestResult *parseNext();
    void parse();
    std::vector<LitTestResult *> getResults() const;

private:
    std::istream &_stream;
    std::vector<LitTestResult *> _results;
    char _line[1024];
    char _ch;

    char get();
    void next();
    bool is(char const *any);
    bool isEol();
    bool isEof();
    bool expect(std::string const &str, bool fail = true);
    bool isLogDelineator();
    bool isLogDelineator(char *line);
    bool isResultCode(LitResultCode *resultCode = nullptr);
    bool isTestName(std::string *testName = nullptr);
    bool isOutput(std::string *output);
    bool isNumber(int *number);
    void passEndOfLine();
};

} // namespace llvmPy

#endif // __cplusplus
