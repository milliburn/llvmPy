#pragma once
#include <string>
#include <istream>
#include <vector>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class PythonImpl;

class Python {
public:
    Python();
    virtual ~Python();

    /** Initialize the Python runtime. Must be called before
     *  run___() methods. */
    void start(std::vector<std::string> const &argv);

    /** Set an implementation-specific option (-X argument). */
    void addImplOption(std::string const &option);

    int runStdin();
    int runCommand(std::istream &command);
    int runScript(std::string const &path);

private:
    std::unique_ptr<PythonImpl> _impl;
};

} // namespace llvmPy
#endif // __cplusplus
