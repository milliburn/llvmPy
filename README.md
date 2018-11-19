llvmPy
===

llvmPy is a hobbyist JIT compiler for Python 3 based on the LLVM compiler infrastructure. 

It implements an extremely narrow subset of the language and its runtime libraries, as the author's main purpose is to study compiler front-end engineering, LLVM, and testing techniques. Development follows no specific plan or timeline.

# Requirements

- LLVM (tested with 7.0.0).
- Python 3 on the PATH.
- The [LLVM integration testing tool (lit)](https://pypi.org/project/lit/) on the PATH.

Run tests with `lit -v test`, ensuring that LLVM and the build directory are in the PATH (the tests will call the `llvmPy` executable directly, as well as LLVM's `FileCheck` and `python3` in some cases). The Catch2-based tests in the `llvmPyTest` binary integrate this functionality via the llvmPyLitParse library, producing lit test reports alongside compiled tests. Tests are fork-intensive.

# Configuration

Add the following to CMake options (replacing paths in accordance with your system):

```
-DLLVM_DIR=/opt/homebrew/Cellar/llvm/7.0.0/lib/cmake/llvm/
```
