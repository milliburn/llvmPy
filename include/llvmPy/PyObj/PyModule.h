#pragma once

#include <llvmPy/PyObj/PyObj.h>

#ifdef __cplusplus
namespace llvmPy {

struct Frame;
class Scope;

class PyModule : public PyObj {
public:
    std::string py__str__() override;

    PyObj *py__getattr__(std::string const &name) override;

    void py__setattr__(std::string const &name, PyObj &obj) override;

    std::string const &getName() const;

    std::string const &getPath() const;

private:
    // Frame const &frame;

    // Scope const &scope;
};

} // namespace llvmPy
#endif // __cplusplus
