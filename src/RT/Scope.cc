#include <assert.h>
#include <llvmPy/RT/Scope.h>
using namespace llvmPy;

Scope::Scope()
: _parent(nullptr)
{
}

Scope::Scope(Scope &parent)
: _parent(&parent)
{
}

Scope::~Scope() = default;

bool
Scope::hasParent() const
{
    return _parent != nullptr;
}

Scope &
Scope::getParent() const
{
    assert(hasParent());
    return *_parent;
}
