#pragma once

#include <cstdint>
#include <type_traits>

#include "_cxx17.h"

#if _CXX17_AVAILABLE
#include <algorithm>
#endif

#include "static_class.h"
#include "runtime_array.h"

namespace common_lib {
    namespace utilities {

        struct IntHelper {

            IMPLEMENT_STATIC_CLASS(IntHelper);

            static int32_t divideByRoundUp(int32_t value, int32_t divisor);

            static uint8_t combineNibbles(uint8_t high, uint8_t low);

            static int32_t getNextMultiple(int32_t value, int32_t multiple);

            static uint8_t getHighNibble(uint8_t value);

            static uint8_t getLowNibble(uint8_t value);

            static int8_t getHighNibbleSigned(uint8_t value);

            static int8_t getLowNibbleSigned(int8_t value);

            static array_ptr<int32_t> getPrimes(int32_t maxPrime);

            // https://stackoverflow.com/a/4609795
            template<typename T, typename TValue = typename std::enable_if<std::is_signed<T>::value, T>::type>
            static int sgn(const T &value) {
                return (TValue(0) < value) - (value < TValue(0));
            }

#if _CXX17_AVAILABLE

            template<typename T>
            static constexpr const T &clamp(const T &value, const T &min, const T &max) {
                return std::clamp<T>(value, min, max);
            }

#else

            template<typename T>
            static constexpr const T &clamp(const T &value, const T &min, const T &max) {
                assert(!(max < min));
                return value < min ? min : (max < value ? max : value);
            }

#endif

        };

    }
}
