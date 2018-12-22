#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include "iterator_range.h"

#define IMPLEMENT_ITERABLE_MEMBER(T, prefix, member) \
    public: \
        using prefix##_iterator = T *; \
        using prefix##_const_iterator = T const *; \
        prefix##_iterator prefix##_begin() { return member.begin(); } \
        prefix##_iterator prefix##_end() { return member.end(); } \
        iterator_range<prefix##_iterator> prefix() { return member.iterate(); } \
        prefix##_const_iterator prefix##_begin() const { return member.begin(); } \
        prefix##_const_iterator prefix##_end() const { return member.end(); } \
        iterator_range<prefix##_const_iterator> prefix() const { return member.iterate(); } \
        std::vector<std::shared_ptr<T>> const prefix##_ptrs() { return member.vectorized(); } \
        std::vector<std::shared_ptr<T>> const prefix##_ptrs() const { return member.vectorized(); } \
    private: \
        llvmPy::iterable_member<T> member;

#ifdef __cplusplus
namespace llvmPy {

template<typename T>
class iterable_member {
public:

    using iterator = T *;
    using const_iterator = T const *;

    iterator begin() {
        return *_pointers.data();
    }

    iterator end() {
        return *_pointers.data() + _pointers.size();
    }

    iterator_range<iterator> iterate() {
        return make_range(begin(), end());
    }

    const_iterator begin() const {
        return const_cast<iterable_member<T> *>(this)->begin();
    }

    const_iterator end() const {
        return const_cast<iterable_member<T> *>(this)->end();
    }

    iterator_range<const_iterator> iterate() const {
        return const_cast<iterable_member<T> *>(this)->iterate();
    }

    void push_back(std::shared_ptr<T> ref) {
        assert(ref);
        _pointers.emplace_back(ref.get());
        _references.emplace_back(std::move(ref));
    }

    std::vector<std::shared_ptr<T>> const vectorized() {
        return std::vector<std::shared_ptr<T>>(_references);
    }

    std::vector<std::shared_ptr<T>> const vectorized() const {
        return _references;
    }

    size_t size() const {
        return _references.size();
    }

private:

    std::vector<std::shared_ptr<T>> _references;

    std::vector<T *> _pointers;

};

} // namespace llvmPy
#endif // __cplusplus
