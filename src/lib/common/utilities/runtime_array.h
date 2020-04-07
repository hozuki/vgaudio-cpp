#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <list>
#include <memory>
#include <cstring>
#include <cassert>

#include "type_sys.h"
#include "StrHelper.h"
#include "static_class.h"
#include "traits.h"

#include "private/runtime_array_decl.h"
#include "private/runtime_array_container.h"
#include "private/runtime_array_wrapper.h"

namespace common_lib::utilities {

    template<typename T>
    using array_ptr = typename std::shared_ptr<runtime_array<T>>;

    template<typename T, size_t count>
    using narray_ptr = typename std::shared_ptr<std::array<T, count>>;

    template<typename T>
    array_ptr<T> make_array_dynamic(const T *data, size_t count, bool copy, bool autoDelete) {
        return std::make_shared<runtime_array_container<T>>(data, count, copy, autoDelete);
    }

    template<typename T>
    array_ptr<T> make_array_dynamic(const T *data, size_t count, bool copy = false) {
        return make_array_dynamic(data, count, copy, true);
    }

    template<typename T>
    array_ptr<T> make_array_dynamic(size_t count) {
        return make_array_dynamic<T>(nullptr, count, false, true);
    }

    // https://zh.cppreference.com/w/cpp/language/sizeof...
    template<typename... Ts>
    constexpr auto make_array_static_with(Ts &&... ts)
    -> narray_ptr<std::common_type_t<Ts...>, sizeof...(ts)> {
        auto arr = new std::array<std::common_type_t<Ts...>, sizeof...(ts)> {std::forward<Ts>(ts)...};
        return std::shared_ptr<typename std::remove_reference<decltype(*arr)>::type>(arr);
    }

    namespace __internal {

        template<typename... Ts, typename U = std::common_type_t<Ts...>>
        static void make_array_dynamic_with_helper([[maybe_unused]] const array_ptr<U> &arr, [[maybe_unused]] size_t index) {
            // Do nothing, end of recursion
        }

        template<typename T, typename... Ts, typename U = std::common_type_t<T, Ts...>>
        static void make_array_dynamic_with_helper(const array_ptr<U> &arr, size_t index, T t, Ts &&... ts) {
            (*arr)[index] = t;
            make_array_dynamic_with_helper(arr, index + 1, ts...);
        }

    }

    template<typename... Ts, typename U = std::common_type_t<Ts...>>
    static array_ptr<U> make_array_dynamic_with(Ts &&... ts) {
        const size_t count = sizeof...(ts);
        auto arr = make_array_dynamic<U>(count);

        // TODO: Edge cases when index overflows...
        __internal::make_array_dynamic_with_helper(arr, 0, ts...);

        return arr;
    }

    template<typename T, size_t count>
    narray_ptr<T, count> make_array_static() {
        return std::make_shared<std::array<T, count>>();
    }

}
