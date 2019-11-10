#include "../utilities/defer.h"
#include "Stream.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

int32_t Stream::read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    throw std::runtime_error("not supported");
}

int32_t Stream::read(const array_ptr<uint8_t> &buffer, int32_t offset, int32_t count) {
    if (!canRead()) {
        throw std::runtime_error("not supported");
    }

    validateBufferParameters(buffer->data(), buffer->size(), offset, count);

    if (count == 0) {
        return 0;
    } else {
        return read(buffer->data(), buffer->size(), offset, count);
    }
}

int32_t Stream::write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    throw std::runtime_error("not supported");
}

int32_t Stream::write(const array_ptr<uint8_t> &buffer, int32_t offset, int32_t count) {
    if (!canWrite()) {
        throw std::runtime_error("not supported");
    }

    validateBufferParameters(buffer->data(), buffer->size(), offset, count);

    if (count == 0) {
        return 0;
    } else {
        return write(buffer->data(), buffer->size(), offset, count);
    }
}

void Stream::flush() {
    throw std::runtime_error("not supported");
}

void Stream::seek(int64_t offset, SeekOrigin origin) {
    if (!canSeek()) {
        throw std::runtime_error("not supported");
    }

    int64_t newPosition;

    switch (origin) {
        case SeekOrigin::Begin:
            newPosition = offset;
            break;
        case SeekOrigin::Current:
            newPosition = getPosition() + offset;
            break;
        case SeekOrigin::End:
            newPosition = getLength() - offset;
            break;
    }

    setPosition(newPosition);
}

int64_t Stream::getPosition() {
    throw std::runtime_error("not supported");
}

void Stream::setPosition(int64_t position) {
    throw std::runtime_error("not supported");
}

int64_t Stream::getLength() {
    throw std::runtime_error("not supported");
}

void Stream::setLength(int64_t length) {
    throw std::runtime_error("not supported");
}

bool Stream::canRead() const {
    return false;
}

bool Stream::canWrite() const {
    return false;
}

bool Stream::canSeek() const {
    return false;
}

void Stream::validateBufferParameters(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    assert(buffer != nullptr);
    assert(offset >= 0);
    assert(count >= 0);
    assert(bufferSize - offset >= count);
}

constexpr size_t DefaultCopyBufferSize = 4096;

void Stream::copyTo(const shared_ptr<Stream> &destination) {
    copyTo(destination, DefaultCopyBufferSize);
}

void Stream::copyTo(const shared_ptr<Stream> &destination, size_t bufferSize) {
    if (!this->canRead()) {
        throw std::runtime_error("cannot read this stream");
    }
    if (!destination->canWrite()) {
        throw std::runtime_error("cannot write destination stream");
    }

    auto buffer = new uint8_t[bufferSize];

    auto_defer([&]() {
        delete[] buffer;
    });

    int32_t read;
    do {
        read = this->read(buffer, bufferSize, 0, bufferSize);

        if (read > 0) {
            destination->write(buffer, bufferSize, 0, read);
        }
    } while (read > 0);
}
