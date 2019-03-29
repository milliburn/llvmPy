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
: _resultCode(resultCode),
  _suiteName(suiteName),
  _testName(testName),
  _output(output),
  _currentProgress(currentProgress),
  _maxProgress(maxProgress)
{
}

LitResultCode
LitTestResult::getResultCode() const
{
    return _resultCode;
}

std::string
LitTestResult::getSuiteName() const
{
    return _suiteName;
}

std::string
LitTestResult::getTestName() const
{
    return _testName;
}

std::string
LitTestResult::getOutput() const
{
    return _output;
}

int
LitTestResult::getCurrentProgress() const
{
    return _currentProgress;
}

int
LitTestResult::getMaxProgress() const
{
    return _maxProgress;
}

LitParser::LitParser(std::istream &stream)
: _stream(stream)
{
    next();
}

LitParser::LitParser(std::string const &input)
: _stream(*new std::istringstream(input)) // XXX
{
    next();
}

std::vector<LitTestResult *>
LitParser::getResults() const
{
    return _results;
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

        isOutput(&output);

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
    while (true) {
        LitTestResult *result = parseNext();
        if (result) {
            _results.push_back(result);
        }

        if (isEof()) {
            break;
        }
    }
}

char
LitParser::get()
{
    return _ch;
}

void
LitParser::next()
{
    _ch = static_cast<char>(_stream.get());
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
    return _ch == static_cast<char>(eof);
}

bool
LitParser::expect(std::string const &str, bool fail)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != get()) {
            if (!fail) return false;
            throw std::runtime_error("Expected: " + str);
        }

        next();
    }

    return true;
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

bool
LitParser::isOutput(std::string *output)
{
    if (!isLogDelineator()) {
        return false;
    }

    bool isOutput = expect(" TEST ", false);
    passEndOfLine();

    if (!isOutput) {
        return false;
    }

    std::stringstream ss;

    // Bit of a hack; this "ungets" the character of the original stream.
    ss << get();

    while (true) {
        _stream.getline(_line, sizeof(_line));
        if (isLogDelineator(_line)) {
            break;
        } else {
            ss << _line << "\n";
        }
    }

    *output = ss.str();
    next();
    return true;
}

bool
LitParser::isLogDelineator()
{
    int i = 0;

    while (is("*")) {
        next();
        i += 1;
    }

    return i >= 4;
}

bool
LitParser::isLogDelineator(char *line)
{
    int i = 0;

    while (line[i] == '*') {
        i += 1;
    }

    return i >= 4;
}

std::ostream &
llvmPy::operator<<(std::ostream &s, llvmPy::LitResultCode const &code)
{
    switch (code) {
    case LitResultCode::PASS: s << "PASS"; break;
    case LitResultCode::XFAIL: s << "XFAIL"; break;
    case LitResultCode::XPASS: s << "XPASS"; break;
    case LitResultCode::FAIL: s << "FAIL"; break;
    case LitResultCode::UNRESOLVED: s << "UNRESOLVED"; break;
    case LitResultCode::UNSUPPORTED: s << "UNSUPPORTED"; break;
    }

    return s;
}
