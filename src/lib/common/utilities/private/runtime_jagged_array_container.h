#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "../runtime_array.h"

namespace common_lib::utilities {

    using namespace std;

    template<typename T, size_t Rank>
    struct runtime_jagged_array;

    template<typename T, size_t Rank>
    using jarray_ptr = shared_ptr<runtime_jagged_array<T, Rank>>;

    template<typename T>
    struct runtime_jagged_array<T, 0> {
        // Invalid struct
    };

    template<typename T>
    struct runtime_jagged_array<T, 1> {
        // Invalid struct
    };

    template<typename T>
    struct runtime_jagged_array<T, 2> {

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

        explicit runtime_jagged_array(size_t count) {
            auto arrays = make_array_dynamic<array_ptr<value_type>>(count);
            _arrays = arrays;
        }

        runtime_jagged_array()
            : runtime_jagged_array(0) {
        }

        ~runtime_jagged_array() = default;

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

    template<typename T, size_t Rank>
    struct runtime_jagged_array {

        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;

    private:

        array_ptr<jarray_ptr<value_type, Rank - 1>> _arrays;

        void checkIndex(difference_type index) const noexcept(false) {
            if (index < 0 || index >= size()) {
                throw std::out_of_range(StrHelper::format("index %zu out of range", index));
            }
        }

    public:

        explicit runtime_jagged_array(size_t count) {
            auto arrays = make_array_dynamic<jarray_ptr<value_type, Rank - 1>>(count);
            _arrays = arrays;
        }

        runtime_jagged_array()
            : runtime_jagged_array(0) {
        }

        ~runtime_jagged_array() = default;

        [[nodiscard]]
        constexpr size_type rank() const {
            return Rank;
        }

        const jarray_ptr<value_type, Rank - 1> &at(difference_type index1) const {
            checkIndex(index1);
            return _arrays->at(index1);
        }

        jarray_ptr<value_type, Rank - 1> &at(difference_type index1) {
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

        const jarray_ptr<value_type, Rank - 1> &operator[](difference_type index1) const {
            return (*_arrays)[index1];
        }

        jarray_ptr<value_type, Rank - 1> &operator[](difference_type index1) {
            return (*_arrays)[index1];
        }

        typename runtime_array<jarray_ptr<value_type, Rank - 1>>::iterator begin() {
            return _arrays->begin();
        }

        typename runtime_array<jarray_ptr<value_type, Rank - 1>>::iterator end() {
            return _arrays->end();
        }

        typename runtime_array<jarray_ptr<value_type, Rank - 1>>::const_iterator cbegin() {
            return _arrays->cbegin();
        }

        typename runtime_array<jarray_ptr<value_type, Rank - 1>>::const_iterator cend() {
            return _arrays->cend();
        }

    };

    template<typename T, size_t Rank, typename TDim, typename... TDims>
    jarray_ptr<T, Rank> make_jagged_array_dynamic_helper__(TDim dim, TDims... dims) {
        static_assert(Rank > 2);
        static_assert(std::is_convertible<TDim, size_t>::value);

        auto arr = make_shared<runtime_jagged_array<T, Rank>>(dim);

        for (auto i = 0; i < dim; i += 1) {
            auto a = make_jagged_array_dynamic_helper__<T, Rank - 1, TDims...>(dims...);
            (*arr)[i] = a;
        }

        return arr;
    }

    template<typename T, size_t Rank, typename TDim1, typename TDim2>
    jarray_ptr<T, Rank> make_jagged_array_dynamic_helper__(TDim1 dim1, TDim2 dim2) {
        static_assert(Rank == 2);
        static_assert(std::is_convertible<TDim1, size_t>::value);
        static_assert(std::is_convertible<TDim2, size_t>::value);

        auto arr = make_shared<runtime_jagged_array<T, Rank>>(dim1);

        for (auto i = 0; i < dim1; i += 1) {
            auto a = make_array_dynamic<T>(dim2);
            (*arr)[i] = a;
        }

        return arr;
    }

    template<typename T, size_t Rank, typename... TDims>
    jarray_ptr<T, Rank> make_jagged_array_dynamic(TDims... dims) {
        static_assert(Rank >= 2);
        static_assert(sizeof...(dims) == Rank);

        return make_jagged_array_dynamic_helper__<T, Rank, TDims...>(dims...);
    }

}
