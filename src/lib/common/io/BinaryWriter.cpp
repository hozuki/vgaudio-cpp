#include <algorithm>

#include "BinaryWriter.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

BinaryWriter::BinaryWriter(const shared_ptr<Stream> &stream)
    : _stream(stream) {
}

shared_ptr<Stream> BinaryWriter::getBaseStream() const {
    return _stream;
}

int32_t BinaryWriter::write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    return _stream->write(buffer, bufferSize, offset, count);
}

int32_t BinaryWriter::write(const array_ptr<uint8_t> &buffer, int32_t count) {
    return write(buffer, 0, count);
}

int32_t BinaryWriter::write(const array_ptr<uint8_t> &buffer, int32_t offset, int32_t count) {
    if (offset < 0) {
        throw std::invalid_argument("invalid array start");
    }

    if (count < 0) {
        throw std::invalid_argument("invalid count");
    }

    const auto toWrite = std::min<int32_t>(count, buffer->size() - offset);

    if (toWrite == 0) {
        return 0;
    } else {
        return write(buffer->data(), buffer->size(), offset, toWrite);
    }
}

#define DEFINE_WRITE_METHOD(value_type) \
    int32_t BinaryWriter::write(value_type value) { \
        return write(&value, sizeof(value), 0, sizeof(value)); \
    }

DEFINE_WRITE_METHOD(uint8_t)

DEFINE_WRITE_METHOD(int8_t)

DEFINE_WRITE_METHOD(uint16_t)

DEFINE_WRITE_METHOD(int16_t)

DEFINE_WRITE_METHOD(uint32_t)

DEFINE_WRITE_METHOD(int32_t)

DEFINE_WRITE_METHOD(uint64_t)

DEFINE_WRITE_METHOD(int64_t)

DEFINE_WRITE_METHOD(float)

DEFINE_WRITE_METHOD(double)

int32_t BinaryWriter::write(bool value) {
    return write(static_cast<uint8_t>(value ? 1 : 0));
}

int32_t BinaryWriter::writeNTString(const char *str) {
    return writeNTString(str, SIZE_MAX);
}

int32_t BinaryWriter::writeNTString(const char *str, size_t maxSize) {
    const auto len = strlen(str);
    const auto size = std::min(len + 1, maxSize);

    if (size == 0) {
        return 0;
    } else {
        return write(str, size, 0, size);
    }
}

int32_t BinaryWriter::writeNTString(const std::string &str) {
    const auto length = str.length();

    return writeNTString(str.c_str(), length + 1);
}

int32_t BinaryWriter::writeFixedString(const char *str, size_t size) {
    return write(str, size, 0, size);
}

int32_t BinaryWriter::writeFixedString(const std::string &str) {
    return writeFixedString(str.c_str(), str.length());
}
