#pragma once

#include <cstdint>
#include <array>

#include "runtime_array.h"

namespace common_lib::utilities {

    using namespace std;

    template<typename T>
    struct jagged_runtime_array_2 {

        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

    private:

        array_ptr <array_ptr<value_type>> _arrays;

        void checkIndex(difference_type index) const noexcept(false) {
            if (index < 0 || index >= size()) {
                throw std::out_of_range(StrHelper::format("index %zu out of range", index));
            }
        }

    public:

        explicit jagged_runtime_array_2(size_t count) {
            auto arrays = make_array_dynamic<array_ptr<value_type>>(count);
            _arrays = arrays;
        }

        jagged_runtime_array_2()
            : jagged_runtime_array_2(0) {
        }

        ~jagged_runtime_array_2() = default;

        [[nodiscard]]
        constexpr size_type rank() const {
            return 2;
        }

        const array_ptr <value_type> &at(difference_type index1) const {
            checkIndex(index1);
            return _arrays->at(index1);
        }

        array_ptr <value_type> &at(difference_type index1) {
            checkIndex(index1);
            return _arrays->at(index1);
        }

        [[nodiscard]]
        bool empty() const {
            return _arrays->empty();
        }

        [[nodiscard]]
        size_type size() const {
            return _arrays->size();
        }

        [[nodiscard]]
        constexpr size_type elementSize() const {
            return sizeof(value_type);
        }

        const array_ptr <value_type> &operator[](difference_type index1) const {
            return (*_arrays)[index1];
        }

        array_ptr <value_type> &operator[](difference_type index1) {
            return (*_arrays)[index1];
        }

        typename runtime_array<array_ptr < value_type>>::

        iterator begin() {
            return _arrays->begin();
        }

        typename runtime_array<array_ptr < value_type>>::

        iterator end() {
            return _arrays->end();
        }

        typename runtime_array<array_ptr < value_type>>::

        const_iterator cbegin() {
            return _arrays->cbegin();
        }

        typename runtime_array<array_ptr < value_type>>::

        const_iterator cend() {
            return _arrays->cend();
        }

    };

    template<typename T>
    using jarray2_ptr = typename std::shared_ptr<jagged_runtime_array_2<T>>;

    template<typename T, size_t dim1, size_t dim2>
    using jnarray2_ptr = narray_ptr <narray_ptr<T, dim2>, dim1>;

    template<typename T>
    jarray2_ptr<T> make_jagged_array_2_dynamic(size_t dim1) {
        return std::make_shared<jagged_runtime_array_2<T>>(dim1);
    }

    template<typename T>
    jarray2_ptr<T> make_jagged_array_2_dynamic(size_t dim1, size_t dim2) {
        auto arr = std::make_shared<jagged_runtime_array_2<T>>(dim1);

        for (auto i = 0; i < dim1; i += 1) {
            auto a = make_array_dynamic<T>(dim2);
            (*arr)[i] = a;
        }

        return arr;
    }

    template<typename T, size_t dim1, size_t dim2>
    jnarray2_ptr<T, dim1, dim2> make_jagged_array_2_static() {
        auto arr = std::make_shared<std::array<narray_ptr<T, dim2>, dim1>>();

        for (size_t i = 0; i < dim1; i += 1) {
            (*arr)[i] = make_array_static<T, dim2>();
        }

        return arr;
    }

}
