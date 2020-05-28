#pragma once

#include <cstdint>

#include "static_class.h"

namespace common_lib {
    namespace utilities {

        struct BitHelper {

            IMPLEMENT_STATIC_CLASS(BitHelper);

            static uint32_t bitReverse32(uint32_t value);

            static int32_t bitReverse32(int32_t value);

            static uint32_t bitReverse32(uint32_t value, int32_t bitCount);

            static int32_t bitReverse32(int32_t value, int32_t bitCount);

            static int32_t signExtend32(int32_t value, int32_t bitCount);

            static uint8_t getHighNibble(uint8_t value);

            static uint8_t getLowNibble(uint8_t value);

        };

    }
}
