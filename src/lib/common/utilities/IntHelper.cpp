#include <cmath>

#include "IntHelper.h"

using namespace common_lib::utilities;

int32_t IntHelper::divideByRoundUp(int32_t value, int32_t divisor) {
    return (int32_t)std::ceil((double)value / divisor);
}

uint8_t IntHelper::combineNibbles(uint8_t high, uint8_t low) {
    uint32_t h = high, l = low;
    return static_cast<uint8_t>((h << 4u) | l);
}

int32_t IntHelper::getNextMultiple(int32_t value, int32_t multiple) {
    if (multiple <= 0) {
        return value;
    }

    if (value % multiple == 0) {
        return value;
    }

    return value + multiple - value % multiple;
}
