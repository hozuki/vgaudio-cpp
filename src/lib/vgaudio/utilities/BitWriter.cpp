#include <cassert>
#include <cinttypes>

#include "../../common/utilities/IntHelper.h"
#include "BitWriter.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::utilities;

BitWriter::BitWriter(const array_ptr<uint8_t> &buffer)
    : _position(0) {
    SetBuffer(buffer);
}

void BitWriter::SetBuffer(const array_ptr<uint8_t> &buffer, size_t position) {
    _buffer = buffer;
    setPosition(position);
}

array_ptr<uint8_t> BitWriter::getBuffer() const {
    return _buffer;
}

size_t BitWriter::getLengthBits() const {
    return _buffer->size() * 8;
}

void BitWriter::setPosition(size_t position) {
    _position = position;
}

size_t BitWriter::getPosition() const {
    return _position;
}

size_t BitWriter::getRemaining() const {
    return getLengthBits() - getPosition();
}

void BitWriter::alignPosition(int32_t multiple) {
    const auto newPosition = IntHelper::getNextMultiple(getPosition(), multiple);
    const int32_t bitCount = newPosition - getPosition();
    write(0, bitCount);
}

void BitWriter::write(int32_t value, int32_t bitCount) {
    assert(0 <= bitCount && bitCount <= 32);

    const auto remaining = getRemaining();

    if (bitCount > remaining) {
        throw std::invalid_argument(StrHelper::format("no enough bits left in the output buffer (required %" PRId32 ", remaining %zu)", bitCount, remaining));
    }

    const int32_t byteIndex = getPosition() / 8;
    const int32_t bitIndex = getPosition() % 8;
    const auto &buffer = getBuffer();

    if (bitCount <= 9 && remaining >= 16) {
        const int32_t outValue = ((value << (16 - bitCount)) & 0xffff) >> bitIndex;

        (*buffer)[byteIndex] |= static_cast<uint8_t>(outValue >> 8);
        (*buffer)[byteIndex + 1] = static_cast<uint8_t>(outValue);
    } else if (bitCount <= 17 && remaining >= 24) {
        const int32_t outValue = ((value << (24 - bitCount)) & 0xffffff) >> bitIndex;

        (*buffer)[byteIndex] |= static_cast<uint8_t>(outValue >> 16);
        (*buffer)[byteIndex + 1] = static_cast<uint8_t>(outValue >> 8);
        (*buffer)[byteIndex + 2] = static_cast<uint8_t>(outValue);
    } else if (bitCount <= 25 && remaining >= 32) {
        const int32_t outValue = ((value << (32 - bitCount)) & 0xffffffff) >> bitIndex;

        (*buffer)[byteIndex] |= static_cast<uint8_t>(outValue >> 24);
        (*buffer)[byteIndex + 1] |= static_cast<uint8_t>(outValue >> 16);
        (*buffer)[byteIndex + 2] = static_cast<uint8_t>(outValue >> 8);
        (*buffer)[byteIndex + 3] = static_cast<uint8_t>(outValue);
    } else {
        writeFallback(value, bitCount);
    }

    setPosition(getPosition() + bitCount);
}

void BitWriter::writeFallback(int32_t value, int32_t bitCount) {
    int32_t byteIndex = getPosition() / 8;
    int32_t bitIndex = getPosition() % 8;
    const auto &buffer = getBuffer();

    while (bitCount > 0) {
        if (bitIndex >= 8) {
            bitIndex = 0;
            byteIndex += 1;
        }

        const int32_t toShift = 8 - bitIndex - bitCount;
        const int32_t shifted = toShift < 0 ? value >> -toShift : value << toShift;

        const int32_t bitsToWrite = std::min(bitCount, 8 - bitIndex);

        const int32_t mask = ((1 << bitsToWrite) - 1) << (8 - bitIndex - bitsToWrite);
        int32_t outByte = (*buffer)[byteIndex] & ~mask;

        outByte |= shifted & mask;
        (*buffer)[byteIndex] = static_cast<uint8_t>(outByte);

        bitIndex += bitsToWrite;
        bitCount -= bitsToWrite;
    }
}
