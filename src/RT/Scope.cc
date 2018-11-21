#include <assert.h>
#include <llvmPy/RT/Scope.h>
using namespace llvmPy;

Scope::Scope()
: parent_(nullptr)
{
}

Scope::Scope(Scope &parent)
: parent_(&parent)
{
}

Scope::~Scope() = default;

bool
Scope::hasParent() const
{
    return parent_ != nullptr;
}

Scope &
Scope::getParent() const
{
    assert(hasParent());
    return *parent_;
}
