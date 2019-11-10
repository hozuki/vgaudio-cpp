#pragma once

#include <cstdint>

#include "static_class.h"

namespace common_lib::utilities {

    struct IntHelper {

        IMPLEMENT_STATIC_CLASS(IntHelper);

        static int32_t divideByRoundUp(int32_t value, int32_t divisor);

        static uint8_t combineNibbles(uint8_t high, uint8_t low);

        static int32_t getNextMultiple(int32_t value, int32_t multiple);

    };

}
