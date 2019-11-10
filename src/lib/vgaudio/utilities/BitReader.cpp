#include <cmath>
#include <cassert>

#include "../../common/utilities/BitHelper.h"
#include "../../common/utilities/IntHelper.h"

#include "BitReader.h"

using namespace common_lib::utilities;
using namespace vgaudio::utilities;

void BitReader::SetBuffer(const array_ptr<uint8_t> &buffer) {
    _buffer = buffer;
    setPosition(0);
}

array_ptr<uint8_t> BitReader::getBuffer() const {
    return _buffer;
}

size_t BitReader::getLengthBits() const {
    return getBuffer()->size() * 8;
}

void BitReader::setPosition(size_t position) {
    _position = position;
}

size_t BitReader::getPosition() const {
    return _position;
}

size_t BitReader::getRemaining() const {
    return getLengthBits() - getPosition();
}

BitReader::BitReader(const array_ptr<uint8_t> &buffer)
    : _position(0) {
    SetBuffer(buffer);
}

int32_t BitReader::readInt(int32_t bitCount) {
    int32_t value = PeekInt(bitCount);
    setPosition(getPosition() + bitCount);
    return value;
}

int32_t BitReader::readSignedInt(int32_t bitCount) {
    int32_t value = PeekInt(bitCount);
    setPosition(getPosition() + bitCount);
    return BitHelper::signExtend32(value, bitCount);
}

bool BitReader::readBool() {
    return readInt(1) != 0;
}

int32_t BitReader::readOffsetBinary(int32_t bitCount, OffsetBias bias) {
    int32_t offset = (1 << (bitCount - 1)) - static_cast<int32_t>(bias);
    int32_t value = PeekInt(bitCount) - offset;
    setPosition(getPosition() + bitCount);
    return value;
}

void BitReader::alignPosition(int32_t multiple) {
    setPosition(IntHelper::getNextMultiple(getPosition(), multiple));
}

int32_t BitReader::PeekIntFallback(int32_t bitCount) {
    int32_t value = 0;

    int32_t byteIndex = getPosition() / 8;
    int32_t bitIndex = getPosition() % 8;
    const auto &buffer = getBuffer();

    while (bitCount > 0) {
        if (bitIndex >= 8) {
            bitIndex = 0;
            byteIndex += 1;
        }

        int32_t bitsToRead = std::min(bitCount, 8 - bitIndex);
        int32_t mask = 0xff >> bitIndex;
        int32_t currentByte = ((*buffer)[byteIndex] & mask) >> (8 - bitIndex - bitsToRead);

        value = (value << bitsToRead) | currentByte;
        bitIndex += bitsToRead;
        bitCount -= bitsToRead;
    }

    return value;
}

int32_t BitReader::PeekInt(int32_t bitCount) {
    assert(0 <= bitCount && bitCount <= 32);

    const auto remaining = getRemaining();

    if (bitCount > remaining) {
        if (getPosition() >= getLengthBits()) {
            return 0;
        }

        int32_t extraBits = bitCount - remaining;

        return PeekIntFallback(remaining) << extraBits;
    }

    const int32_t byteIndex = getPosition() / 8;
    const int32_t bitIndex = getPosition() % 8;
    const auto &buffer = getBuffer();

    if (bitCount <= 9 && remaining >= 16) {
        int32_t value = ((*buffer)[byteIndex] << 8) | (*buffer)[byteIndex + 1];
        value &= 0xffff >> bitIndex;
        value >>= 16 - bitCount - bitIndex;
        return value;
    }

    if (bitCount <= 17 && remaining >= 24) {
        int32_t value = ((*buffer)[byteIndex] << 16) | ((*buffer)[byteIndex + 1] << 8) | (*buffer)[byteIndex + 2];
        value &= 0xffffff >> bitIndex;
        value >>= 24 - bitCount - bitIndex;
        return value;
    }

    if (bitCount <= 25 && remaining >= 32) {
        int32_t value = ((*buffer)[byteIndex] << 24) | ((*buffer)[byteIndex + 1] << 16) | ((*buffer)[byteIndex + 2] << 8) | (*buffer)[byteIndex + 3];
        value &= 0xffffffff >> bitIndex;
        value >>= 32 - bitCount - bitIndex;
        return value;
    }

    return PeekIntFallback(bitCount);
}
