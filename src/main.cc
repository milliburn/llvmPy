#include <llvmPy/Python.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unistd.h>
#include <build.h>

using namespace llvmPy;
using std::cerr;
using std::cout;
using std::endl;

enum class Mode {
    STDIN,
    COMMAND,
    SCRIPT,
};

struct Options {
    Mode mode;
    std::string program;
    std::vector<std::string> args;
    std::unordered_set<std::string> impl;
};

static int run(Options const &);

int
main(int argc, char **argv)
{
    Options options = {
            .mode = Mode::STDIN,
    };

    char c;

    while ((c = static_cast<char>(getopt(argc, argv, "hVc:X:"))) != -1) {
        bool isDone = false;

        switch (c) {
        case 'h':
            cout << LLVMPY_NAME << endl;
            exit(0);
        case 'V':
            cout << "Python " << LLVMPY_PYTHON << " ("
                 << LLVMPY_NAME << " " << LLVMPY_VERSION << ", "
                 << LLVMPY_TIMESTAMP << ")"
                 << endl;
            exit(0);
        case 'c':
            options.program = optarg;
            options.mode = Mode::COMMAND;
            isDone = true;
            break;
        case 'X':
            std::string impl = optarg;
            options.impl.insert(impl);
            break;
        }

        if (isDone) {
            break;
        }
    }

    if (optind < argc) {
        options.mode = Mode::SCRIPT;
        options.program = argv[optind++];
    }

    while (optind < argc) {
        options.args.emplace_back(argv[optind++]);
    }

    return run(options);
}

static int
run(Options const &options)
{
    Python python;
    std::vector<std::string> argv;
    int exitcode;

    switch (options.mode) {
    case Mode::STDIN:
        argv.emplace_back("");
        break;
    case Mode::COMMAND:
        argv.emplace_back("-c");
        break;
    case Mode::SCRIPT:
        argv.push_back(options.program);
        break;
    }

    for (auto &arg : options.args) {
        argv.push_back(arg);
    }

    for (auto &impl : options.impl) {
        python.addImplOption(impl);
    }

    python.start(argv);

    switch (options.mode) {
    case Mode::STDIN:
        exitcode = python.runStdin();
        break;
    case Mode::COMMAND: {
        std::stringstream ss(options.program);
        exitcode = python.runCommand(ss);
        break;
    }
    case Mode::SCRIPT:
        exitcode = python.runScript(options.program);
        break;
    }

    return exitcode;
}
