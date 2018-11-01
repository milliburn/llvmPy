#include <llvmPy/LitParser.h>
#include <sstream>
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
}

LitTestResult *
LitParser::parseNext()
{
    return nullptr;
}
