#pragma once
#include <string>

#ifdef __cplusplus
namespace llvmPy {

template<typename I, typename O>
class Phase {
public:
    virtual ~Phase() = default;
    virtual O run(I &) = 0;

    std::string const &getName() const { return _name; }

protected:
    Phase(std::string const &name) : _name(name) {}

private:
    std::string const _name;
};

} // namespace llvmPy
#endif // __cplusplus
