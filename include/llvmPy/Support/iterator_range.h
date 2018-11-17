#pragma once
#include <llvm/ADT/iterator_range.h>

#ifdef __cplusplus
namespace llvmPy {

using llvm::iterator_range;
using llvm::make_range;

template<class T>
iterator_range<T> empty_range() {
    return make_range<T>(nullptr, nullptr);
}

} // namespace llvmPy
#endif // __cplusplus
