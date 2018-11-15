#pragma once

#include <assert.h>

#ifdef __cplusplus
namespace llvmPy {

template<typename T>
class Typed {
public:
    // Ensure the class is polymorphic.
    virtual ~Typed() = default;

    static bool classof(Typed const *x) {
        return false;
    }

    T getType() const {
        return type;
    }

    long getTypeValue() const {
        return static_cast<long>(type);
    }

    bool isTypeAbove(T t) const {
        return static_cast<long>(type) >= static_cast<long>(t);
    }

    bool isTypeBelow(T t) const {
        return static_cast<long>(type) < static_cast<long>(t);
    }

    bool isTypeBetween(T a, T b) const {
        return isTypeAbove(a) && isTypeBelow(b);
    }

    bool isType(T t) const {
        return type == t;
    }

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

protected:
    Typed(T type) : type(type) {}

private:
    T type;
};

} // namespace llvmPy
#endif // __cplusplus
