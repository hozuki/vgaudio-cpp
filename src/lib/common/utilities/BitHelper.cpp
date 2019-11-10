#include "BitHelper.h"

using namespace common_lib::utilities;

uint32_t BitHelper::bitReverse32(uint32_t value) {
    auto v = value;
    v = ((v & 0xaaaaaaaau) >> 1u) | ((v & 0x55555555u) << 1u);
    v = ((v & 0xccccccccu) >> 2u) | ((v & 0x33333333u) << 2u);
    v = ((v & 0xf0f0f0f0u) >> 4u) | ((v & 0x0f0f0f0fu) << 4u);
    v = ((v & 0xff00ff00u) >> 8u) | ((v & 0x00ff00ffu) << 8u);
    return (v >> 16u) | (v << 16u);
}

int32_t BitHelper::bitReverse32(int32_t value) {
    const auto v = bitReverse32(*reinterpret_cast<const uint32_t *>(&value));
    return *reinterpret_cast<const int32_t *>(&v);
}

uint32_t BitHelper::bitReverse32(uint32_t value, int32_t bitCount) {
    return bitReverse32(value) >> (32u - bitCount);
}

int32_t BitHelper::bitReverse32(int32_t value, int32_t bitCount) {
    const auto v = bitReverse32(*reinterpret_cast<const uint32_t *>(&value), bitCount);
    return *reinterpret_cast<const int32_t *>(&v);
}

int32_t BitHelper::signExtend32(int32_t value, int32_t bitCount) {
    uint32_t shift = 8 * sizeof(int32_t) - bitCount;
    return (value << shift) >> shift;
}

uint8_t BitHelper::getHighNibble(uint8_t value) {
    return (value >> 4) & 0xf;
}

uint8_t BitHelper::getLowNibble(uint8_t value) {
    return value & 0xf;
}
