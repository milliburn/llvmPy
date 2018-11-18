#include <llvmPy/Lit.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <exception>
#include <sstream>
using namespace llvmPy;
using std::cerr;
using std::endl;

int
llvmPy::lit(
        std::vector<LitTestResult *> &results,
        std::string const &path)
{
    std::string cmd = "lit -v " + path + " 2>&1";

    FILE *pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        throw std::runtime_error("Unable to run " + cmd);
    }

    std::stringstream ss;
    char buffer[128];

    while (!feof(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            ss << buffer;
        }
    }

    // Waits for termination and returns exit status.
    int exitcode = pclose(pipe);

    ss.seekp(0);

    LitParser parser(ss);

    parser.parse();

    results = parser.getResults();

    return exitcode;
}
