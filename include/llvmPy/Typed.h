#pragma once

#ifdef __cplusplus
namespace llvmPy {

template<typename T>
class Typed {
public:
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

protected:
    Typed(T type) : type(type) {}

private:
    T type;
};

} // namespace llvmPy
#endif // __cplusplus
