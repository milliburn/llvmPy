#include <llvmPy/Typed.h>
using namespace llvmPy;

Typed::Typed() noexcept = default;

Typed::Typed(Typed const &copy) noexcept = default;

Typed::Typed(Typed &&move) noexcept = default;

Typed::~Typed() = default;
