#include <llvmPy/LitParser.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <exception>
using namespace llvmPy;

static constexpr int eof = std::istream::traits_type::eof();

LitTestResult::LitTestResult(
        LitResultCode resultCode,
        std::string const &suiteName,
        std::string const &testName,
        std::string const &output,
        int currentProgress,
        int maxProgress)
: resultCode(resultCode),
  suiteName(suiteName),
  testName(testName),
  output(output),
  currentProgress(currentProgress),
  maxProgress(maxProgress)
{
}

LitResultCode
LitTestResult::getResultCode() const
{
    return resultCode;
}

std::string
LitTestResult::getSuiteName() const
{
    return suiteName;
}

std::string
LitTestResult::getTestName() const
{
    return testName;
}

std::string
LitTestResult::getOutput() const
{
    return output;
}

int
LitTestResult::getCurrentProgress() const
{
    return currentProgress;
}

int
LitTestResult::getMaxProgress() const
{
    return maxProgress;
}

LitParser::LitParser(std::istream &stream)
: stream(stream)
{
}

LitParser::LitParser(std::string const &input)
: stream(*new std::istringstream(input)) // XXX
{
    next();
}

std::vector<LitTestResult *>
LitParser::getResults() const
{
    return results;
}

LitTestResult *
LitParser::parseNext()
{
    LitResultCode resultCode;
    std::string suiteName;
    std::string testName;
    std::string output;
    int currentProgress = 0;
    int maxProgress = 0;

    if (isResultCode(&resultCode)) {
        expect(": ");

        if (!isTestName(&suiteName)) {
            throw std::runtime_error("Expected suite name.");
        }

        expect(" :: ");

        if (!isTestName(&testName)) {
            throw std::runtime_error("Expected test name.");
        }

        expect(" (");

        if (!isNumber(&currentProgress)) {
            throw std::runtime_error("Expected current progress.");
        }

        expect(" of ");

        if (!isNumber(&maxProgress)) {
            throw std::runtime_error("Expected max progress.");
        }

        expect(")");

        passEndOfLine();

        return new LitTestResult(
                resultCode,
                suiteName,
                testName,
                output,
                currentProgress,
                maxProgress);
    }

    passEndOfLine();

    return nullptr;
}

void
LitParser::parse()
{
    while (!isEof()) {
        LitTestResult *result = parseNext();
        if (result) {
            results.push_back(result);
        }
    }
}

char
LitParser::get()
{
    return ch;
}

void
LitParser::next()
{
    ch = (char) stream.get();
}

bool
LitParser::is(char const *any)
{
    return !!std::strchr(any, get());
}

bool
LitParser::isEol()
{
    return is("\n\r");
}

bool
LitParser::isEof()
{
    return ch == (char) eof;
}

void
LitParser::expect(std::string const &str)
{
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] != get()) {
            throw std::runtime_error("Expected: " + str);
        }

        next();
    }
}

bool
LitParser::isResultCode(LitResultCode *resultCode)
{
    std::stringstream ss;

    while (!is(":")) {
        ss << get();
        next();
    }

    std::string str = ss.str();

    if (str == "PASS") *resultCode = LitResultCode::PASS;
    else if (str == "XFAIL") *resultCode = LitResultCode::XFAIL;
    else if (str == "FAIL") *resultCode = LitResultCode::FAIL;
    else if (str == "XPASS") *resultCode = LitResultCode::XPASS;
    else if (str == "UNRESOLVED") *resultCode = LitResultCode::UNRESOLVED;
    else if (str == "UNSUPPORTED") *resultCode = LitResultCode::UNSUPPORTED;
    else return false;

    return true;
}

bool
LitParser::isTestName(std::string *testName)
{
    while (is(" ")) {
        next();
    }

    std::stringstream ss;

    while (!is(" (")) {
        ss << get();
        next();
    }

    *testName = ss.str();

    return !testName->empty();
}

bool
LitParser::isNumber(int *number)
{
    std::stringstream ss;

    while (is("0123456789")) {
        ss << get();
        next();
    }

    std::string str = ss.str();

    if (str.empty()) {
        return false;
    }

    *number = std::stoi(str);
    return true;
}

void
LitParser::passEndOfLine()
{
    while (!isEol() && !isEof()) {
        next();
    }

    // Pass duplicate EOLs.
    while (isEol()) {
        next();
    }
}
