#pragma once

#include <type_traits>

namespace common_lib::utilities {

    // https://stackoverflow.com/a/7110361

    template<typename... T>
    struct all_integral;

    template<>
    struct all_integral<> {
        static constexpr bool value = true;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    template<typename T, typename... TRest>
    struct all_integral<T, TRest...> {
        static constexpr bool value = std::is_integral<T>::value && all_integral<TRest...>::value;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    template<typename... T>
    struct all_floating_point;

    template<>
    struct all_floating_point<> {
        static constexpr bool value = true;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    template<typename T, typename... TRest>
    struct all_floating_point<T, TRest...> {
        static constexpr bool value = std::is_floating_point<T>::value && all_floating_point<TRest...>::value;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    template<typename... T>
    struct all_numeric;

    template<>
    struct all_numeric<> {
        static constexpr bool value = true;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    template<typename T, typename... TRest>
    struct all_numeric<T, TRest...> {
        static constexpr bool value = (std::is_integral<T>::value || std::is_floating_point<T>::value) && all_numeric<TRest...>::value;

        constexpr operator bool() const {
            return value;
        }

        constexpr bool operator()() const {
            return value;
        }
    };

    // https://stackoverflow.com/a/26936864
    template<typename T, typename V = void>
    struct _enum_class_test : std::false_type {
    };

    template<typename T>
    struct _enum_class_test<T, decltype((void)+T {})> : std::true_type {
    };

    template<typename T>
    using is_enum_class = std::integral_constant<bool, !_enum_class_test<T>::value && std::is_enum<T>::value>;

}
