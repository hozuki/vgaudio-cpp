#include <cmath>

#include "Crc16.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::utilities;

static narray_ptr<uint16_t, 256> GenerateTable(uint16_t polynomial) {
    auto table = make_array_static<uint16_t, 256>();

    for (size_t i = 0; i < 256; i += 1) {
        auto curByte = static_cast<uint16_t >(i << 8u);

        for (auto j = 0; j < 8; j += 1) {
            auto xorFlag = (curByte & 0x8000u) != 0;
            curByte <<= 1u;

            if (xorFlag) {
                curByte ^= polynomial;
            }
        }

        (*table)[i] = curByte;
    }

    return table;
}

Crc16::Crc16(uint16_t polynomial) {
    _table = GenerateTable(polynomial);
}

uint16_t Crc16::compute(const array_ptr<uint8_t> &data, size_t size) {
    uint16_t crc = 0;
    size_t s = std::min(data->size(), size);

    for (size_t i = 0; i < s; i += 1) {
        crc = static_cast<uint16_t>((crc << 8u) ^ (*_table)[(crc >> 8u) ^ (*data)[i]]);
    }

    return crc;
}

template<size_t N>
uint16_t Crc16::compute(const narray_ptr<uint8_t, N> &data, size_t size) {
    uint16_t crc = 0;
    size_t s = std::min(data->size(), size);

    for (size_t i = 0; i < s; i += 1) {
        crc = static_cast<uint16_t>((crc << 8) ^ (*_table)[(crc >> 8) ^ (*data)[i]]);
    }

    return crc;
}

uint16_t Crc16::compute(const uint8_t *data, size_t size) {
    uint16_t crc = 0;

    for (size_t i = 0; i < size; i += 1) {
        crc = static_cast<uint16_t>((crc << 8u) ^ (*_table)[(crc >> 8u) ^ data[i]]);
    }

    return crc;
}
