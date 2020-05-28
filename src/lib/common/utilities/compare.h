#pragma once

#include <cstddef>
#include <functional>

#include "_cxx17.h"

namespace common_lib {
    namespace utilities {

        template<typename T>
        struct compare;

        template<typename T>
        struct compare {

            _CXX17_CONSTEXPR ptrdiff_t operator()(const T &x, const T &y) const {
                const auto less = std::less<T>();

                if (less(x, y)) {
                    return -1;
                }

                const auto greater = std::greater<T>();

                if (greater(x, y)) {
                    return 1;
                }

                return 0;
            }

        };

    }
}
