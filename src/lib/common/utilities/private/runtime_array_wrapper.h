#pragma once

#include <cstdint>
#include <array>
#include <iterator>
#include <memory>

#include "runtime_array_decl.h"

namespace common_lib::utilities {

    template<typename T, size_t N>
    struct runtime_array_wrapper : public runtime_array<T> {

        typedef runtime_array <T> MyBase;

        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

        using iterator = typename MyBase::iterator;
        using const_iterator = typename MyBase::const_iterator;
        using reverse_iterator = typename MyBase::reverse_iterator;
        using const_reverse_iterator = typename MyBase::const_reverse_iterator;

    private:

        std::shared_ptr<std::array<T, N>> _array;

    public:

        explicit runtime_array_wrapper(const std::shared_ptr<std::array<T, N>> &arr)
            : _array(arr) {
        }

        [[nodiscard]]
        bool empty() const override {
            return _array->empty();
        }

        const_reference at(difference_type index) const override {
            return _array->at(index);
        }

        reference at(difference_type index) override {
            return _array->at(index);
        }

        const_pointer data() const override {
            return _array->data();
        }

        pointer data() override {
            return _array->data();
        }

        const_reference front() const override {
            return _array->front();
        }

        reference front() override {
            return _array->front();
        }

        const_reference back() const override {
            return _array->back();
        }

        reference back() override {
            return _array->back();
        }

        [[nodiscard]]
        size_type size() const override {
            return _array->size();
        }

        [[nodiscard]]
        size_type max_size() const override {
            return _array->max_size();
        }

        const_reference operator[](difference_type index) const override {
            return (*_array)[index];
        }

        reference operator[](difference_type index) override {
            return (*_array)[index];
        }

        iterator begin() override {
            // Here we don't use std::array<T,N>::iterator, because every std library has its own implementation.
            // For example:
            //   GCC: iterator = T *, const_iterator = const T *
            //   MSVC: iterator = std::_Array_iterator<T,N>, const_iterator = std::_Array_const_iterator<T,N>
            // Using raw pointers (like what GCC does) is sufficient but there is no sanity check.
            return _array->data();
        }

        iterator end() override {
            return _array->data() + _array->size();
        }

        const_iterator begin() const override {
            return _array->data();
        }

        const_iterator end() const override {
            return _array->data() + _array->size();
        }

        const_iterator cbegin() const override {
            return const_iterator(begin());
        }

        const_iterator cend() const override {
            return const_iterator(end());
        }

        reverse_iterator rbegin() override {
            return reverse_iterator(begin());
        }

        reverse_iterator rend() override {
            return reverse_iterator(end());
        }

    };

}
