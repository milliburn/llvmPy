#include <assert.h>
#include <llvmPy/RT/Scope.h>
using namespace llvmPy;

Scope::Scope()
: parent(nullptr)
{
}

Scope::Scope(Scope &parent)
: parent(&parent)
{
}

Scope::~Scope() = default;

bool
Scope::hasParent() const
{
    return parent != nullptr;
}

Scope &
Scope::getParent() const
{
    assert(hasParent());
    return *parent;
}
