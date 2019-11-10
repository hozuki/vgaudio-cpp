#include <cstdlib>
#include <cstring>

#include "MemoryStream.h"
#include "../utilities/ArrayHelper.h"
#include "../utilities/defer.h"
#include "BinaryReader.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

BinaryReader::BinaryReader(const shared_ptr<Stream> &stream)
    : _stream(stream) {
}

shared_ptr<Stream> BinaryReader::getBaseStream() const {
    return _stream;
}

int32_t BinaryReader::readBytes(void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    return _stream->read(buffer, bufferSize, offset, count);
}

int32_t BinaryReader::readBytes(const array_ptr<uint8_t> &buffer, int32_t count) {
    return readBytes(buffer, 0, count);
}

int32_t BinaryReader::readBytes(const array_ptr<uint8_t> &buffer, int32_t offset, int32_t count) {
    if (offset < 0) {
        throw std::invalid_argument("invalid array start");
    }

    if (count < 0) {
        throw std::invalid_argument("invalid count");
    }

    const auto toRead = std::min<int32_t>(count, buffer->size() - offset);

    if (toRead == 0) {
        return 0;
    } else {
        return readBytes(buffer->data(), buffer->size(), offset, count);
    }
}

array_ptr<uint8_t> BinaryReader::readBytes(int32_t count) {
    if (count < 0) {
        throw std::invalid_argument("invalid count");
    }

    auto buffer = make_array_dynamic<uint8_t>(count);

    if (count == 0) {
        return buffer;
    }

    int32_t remained = count;
    auto stream = getBaseStream();

    while (true) {
        auto read = readBytes(buffer, count - remained, remained);

        if (read == 0) {
            break;
        }

        remained -= read;

        if (remained == 0) {
            break;
        }

        if (stream->getLength() > 0 && stream->getPosition() >= stream->getLength()) {
            break;
        }
    }

    const auto totalRead = count - remained;

    if (totalRead == count) {
        return buffer;
    }

    assert(totalRead < count);

    auto newBuffer = make_array_dynamic<uint8_t>(totalRead);

    if (totalRead > 0) {
        ArrayHelper::copyByValue(buffer, newBuffer, totalRead);
    }

    return newBuffer;
}

#define DEFINE_READ_METHOD(return_type, type_name) \
    return_type BinaryReader::read##type_name() { \
        return_type result; \
        readBytes(&result, sizeof(result), 0, static_cast<int32_t>(sizeof(result))); \
        return result; \
    }

DEFINE_READ_METHOD(uint8_t, UInt8)

DEFINE_READ_METHOD(int8_t, Int8)

DEFINE_READ_METHOD(uint16_t, UInt16)

DEFINE_READ_METHOD(int16_t, Int16)

DEFINE_READ_METHOD(uint32_t, UInt32)

DEFINE_READ_METHOD(int32_t, Int32)

DEFINE_READ_METHOD(uint64_t, UInt64)

DEFINE_READ_METHOD(int64_t, Int64)

DEFINE_READ_METHOD(float, Single)

DEFINE_READ_METHOD(double, Double)

bool BinaryReader::readBoolean() {
    const auto result = readUInt8();
    return result != 0;
}

std::string BinaryReader::readNTString() {
    auto memoryStream = make_shared<MemoryStream>();

    uint8_t byte;

    do {
        const auto read = readBytes(&byte, 1, 0, 1);

        if (read == 0) {
            break;
        }

        memoryStream->write(&byte, 1, 0, 1);
    } while (byte != 0);

    if (memoryStream->getLength() <= 0) {
        memoryStream->write("\0", 1, 0, 1);
    }

    const auto buffer = memoryStream->toArray();

    return std::string(reinterpret_cast<const char *>(buffer->data()));
}

std::string BinaryReader::readFixedString(size_t size) {
    auto buffer = static_cast<char *>(malloc(size + 1));
    memset(buffer, 0, size + 1);

    auto_defer([&]() {
        free(buffer);
    });

    auto remained = size;

    while (remained > 0) {
        auto bytesRead = readBytes(buffer, size + 1, size - remained, remained);
        remained -= bytesRead;
    }

    return std::string(buffer);
}
