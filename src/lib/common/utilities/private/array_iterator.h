#pragma once

#if 0

#include <cstdint>
#include <iterator>
#include <array>

#include "../_cxx17.h"

namespace common_lib::utilities {

    template<typename T, size_t N = 1, typename TIter = typename std::array<T, N>::iterator>
    struct IteratorWrapper {

        using iterator_category = typename iterator_traits<TIter>::iterator_category;
        using value_type = typename iterator_traits<TIter>::value_type;
        using difference_type = typename iterator_traits<TIter>::difference_type;
        using reference = typename iterator_traits<TIter>::reference;
        using pointer = typename iterator_traits<TIter>::pointer;

    private:

        TIter _iter;

    public:

        explicit IteratorWrapper(const TIter &iter) {
            _iter = iter;
        }

        ~IteratorWrapper() = default;

        _CXX17_ATTR_NODISCARD
        reference operator*() const {
            return _iter.operator*();
        }

       _CXX17_ATTR_NODISCARD
        pointer operator->() const {
            return _iter.operator->();
        }

        IteratorWrapper &operator++() {
            _iter.operator++
            return *this;
        }

        const IteratorWrapper operator++(int) {
            auto tmp = *this;
            _iter.operator++(int);
            return tmp;
        }

        IteratorWrapper &operator--() {
            _iter.operator--();
            return *this;
        }

        const IteratorWrapper operator--(int) {
            auto tmp = *this;
            _iter.operator--(int);
            return tmp;
        }

        IteratorWrapper &operator+=(const ptrdiff_t n) {
            _iter.operator+=(n);
            return *this;
        }

        IteratorWrapper &operator+() {
            _iter.operator+();
            return *this;
        }

       _CXX17_ATTR_NODISCARD
        const IteratorWrapper operator+(const ptrdiff_t n) const {
            auto tmp = *this;
            return tmp += n;
        }

        IteratorWrapper &operator-=(const ptrdiff_t n) {
            _iter.operator-=(n);
            return *this;
        }

        IteratorWrapper &operator-() {
            _iter.operator-();
            return *this;
        }

      _CXX17_ATTR_NODISCARD
        const IteratorWrapper operator-(const ptrdiff_t n) const {
            auto tmp = *this;
            return tmp -= n;
        }

      _CXX17_ATTR_NODISCARD
        const reference operator[](const ptrdiff_t index) const {
            return _iter.operator[](index);
        }

      _CXX17_ATTR_NODISCARD
        reference operator[](const ptrdiff_t index) {
            return _iter.operator[](index);
        }

    };

}

#endif
