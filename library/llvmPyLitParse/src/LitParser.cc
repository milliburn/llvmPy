#include <llvmPy/LitParser.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <exception>
using namespace llvmPy;

LitTestResult::LitTestResult(
        LitResultCode resultCode,
        std::string const &suiteName,
        std::string const &testName,
        std::string const &output)
: resultCode(resultCode),
  suiteName(suiteName),
  testName(testName),
  output(output)
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

LitParser::LitParser(std::string const &input)
: stream(*new std::istringstream(input)) // XXX
{
    next();
}

LitTestResult *
LitParser::parseNext()
{
    LitResultCode resultCode;
    std::string suiteName;
    std::string testName;
    std::string output;

    if (isResultCode(&resultCode)) {
        if (!is(":")) {
            throw std::runtime_error("Expected ':");
        }

        next();

        if (!isTestName(&suiteName)) {
            throw std::runtime_error("Expected suite name");
        }

        for (int i = 0; i < 3; ++i) {
            if (!is(": ")) {
                throw std::runtime_error("Expected test name");
            }

            next();
        }

        if (!isTestName(&testName)) {
            throw std::runtime_error("Expected test name");
        }

        return new LitTestResult(resultCode, suiteName, testName, output);
    } else {
        throw std::runtime_error("Unexpected data!");
    }
}

void
LitParser::next()
{
    ch = (char) stream.get();
}

bool
LitParser::is(char const *any, std::stringstream *ss)
{
    char const *ptr = std::strchr(any, ch);
    if (ptr) {
        return true;
    } else {
        if (ss) {
            (*ss) << ch;
        }

        return false;
    }
}

bool
LitParser::isResultCode(LitResultCode *resultCode)
{
    std::stringstream ss;

    while (!is(":", &ss)) {
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

    while (!is(" (", &ss)) {
        next();
    }

    *testName = ss.str();

    return !testName->empty();
}
