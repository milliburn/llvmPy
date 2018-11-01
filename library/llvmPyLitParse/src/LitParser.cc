#include <llvmPy/LitParser.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <exception>
using namespace llvmPy;

LitTestResult::LitTestResult(
        LitResultCode resultCode,
        std::string const &testName,
        std::string const &output)
: resultCode(resultCode),
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
    std::string testName;
    std::string output;

    if (isResultCode(&resultCode)) {
        // if (!isTestName(&testName)) {
        //     cerr << "Expected test name." << endl;
        //     std::exit(2);
        // }

        return new LitTestResult(resultCode, testName, output);
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
    else if (str == "FAIL") *resultCode = LitResultCode::FAIL;
    else return false;

    return true;
}
