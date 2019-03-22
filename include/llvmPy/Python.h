#pragma once
#include <string>
#include <istream>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

class PythonImpl;

class Python {
public:
    Python();
    virtual ~Python() = default;

    void start(std::vector<std::string> const &argv);

    int runStdin();
    int runCommand(std::istream &command);
    int runScript(std::string const &path);

private:
    std::unique_ptr<PythonImpl> _impl;
};

} // namespace llvmPy
#endif // __cplusplus
