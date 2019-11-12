#pragma once

#include <cstdint>
#include <array>

#include "runtime_array.h"
#include "private/runtime_jagged_array_container.h"

namespace common_lib::utilities {

    // For compatibility
    template<typename T>
    using jarray2_ptr = jarray_ptr<T, 2>;

    template<typename T, size_t dim1, size_t dim2>
    using jnarray2_ptr = narray_ptr<narray_ptr<T, dim2>, dim1>;

    // For compatibility
    template<typename T>
    jarray2_ptr<T> make_jagged_array_2_dynamic(size_t dim1, size_t dim2) {
        return make_jagged_array_dynamic<T, 2>(dim1, dim2);
    }

    // For compatibility
    template<typename T>
    jarray2_ptr<T> make_jagged_array_2_dynamic(size_t dim1) {
        return make_shared<runtime_jagged_array<T, 2>>(dim1);
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
