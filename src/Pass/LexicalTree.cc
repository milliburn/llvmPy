#include <llvmPy/Pass/Analysis/LexicalTree.h>
using namespace llvmPy;

LexicalTree::~LexicalTree() = default;

bool
LexicalTree::isModule() const
{
    return false;
}
