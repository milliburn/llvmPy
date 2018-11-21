#pragma once

#include <assert.h>

#ifdef __cplusplus
namespace llvmPy {

class Typed {
public:
    // Ensure the class is polymorphic.
    virtual ~Typed();

    template<class K>
    bool isa() {
        return !!this->cast<K>();
    }

    template<class K>
    bool isa() const {
        return !!this->cast<K const>();
    }

    template<class K>
    K *cast() {
        return dynamic_cast<K *>(this);
    }

    template<class K>
    K const *cast() const {
        return dynamic_cast<K const *>(this);
    }

    template<class K>
    K &as() {
        auto *ptr = this->cast<K>();
        assert(ptr);
        return *ptr;
    }

    template<class K>
    K const &as() const {
        auto *ptr = this->cast<K const>();
        assert(ptr);
        return *ptr;
    }
};

} // namespace llvmPy
#endif // __cplusplus
