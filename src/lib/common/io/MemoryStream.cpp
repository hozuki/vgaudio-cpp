#include <cassert>
#include <algorithm>
#include <string>

#include "../utilities/ArrayHelper.h"
#include "MemoryStream.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

static constexpr size_t InitialByteBufferSize = 1024;

MemoryStream::MemoryStream()
    : MyClass(InitialByteBufferSize) {
}

MemoryStream::MemoryStream(const array_ptr<uint8_t> &buffer)
    : MyClass(buffer, true) {
}

MemoryStream::MemoryStream(size_t initialCapacity)
    : _writable(true), _canResize(true), _length(0), _position(0) {
    _buffer = make_array_dynamic<uint8_t>(initialCapacity);
}

MemoryStream::MemoryStream(const array_ptr<uint8_t> &buffer, bool writable)
    : _buffer(buffer), _writable(writable), _canResize(false), _length(buffer->size()), _position(0) {
}

MemoryStream::MemoryStream(const void *buffer, size_t count)
    : MyClass(const_cast<void *>(buffer), count, false) {
}

MemoryStream::MemoryStream(void *buffer, size_t count, bool writable)
    : _writable(writable), _canResize(false), _length(count), _position(0) {
    _buffer = make_array_dynamic<uint8_t>(static_cast<uint8_t *>(buffer), count, false, false);
}

int32_t MemoryStream::read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (count < 0) {
        throw std::invalid_argument("invalid count");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    if (count == 0) {
        return 0;
    }

    const auto sizeLeft = getLength() - getPosition();
    const auto toRead = static_cast<size_t>(std::min<int64_t>(count, sizeLeft));

    if (toRead > 0) {
        memcpy(static_cast<uint8_t *>(buffer) + offset, _buffer->data() + getPosition(), toRead);
        setPosition(getPosition() + toRead);
    }

    return toRead;
}

int32_t MemoryStream::write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (!canWrite()) {
        throw std::runtime_error("invalid operation");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    if (count == 0) {
        return 0;
    }

    const auto newPosEnd = getPosition() + count;
    bool shouldUpdateLength = false;

    if (newPosEnd > getLength()) {
        if (_canResize) {
            ensureCapacity(newPosEnd, false);
            shouldUpdateLength = true;
        } else {
            throw std::runtime_error("failed to write to memory stream: trying to access address out of range");
        }
    }

    memcpy(_buffer->data() + getPosition(), static_cast<const uint8_t *>(buffer) + offset, count);

    // Set length, THEN set position. Mind the order.

    if (shouldUpdateLength) {
        setLength(newPosEnd);
    }

    setPosition(newPosEnd);

    return count;
}

void MemoryStream::flush() {
    // Do nothing
}

int64_t MemoryStream::getPosition() {
    return _position;
}

void MemoryStream::setPosition(int64_t position) {
    setPosition(position, false);
}

void MemoryStream::ensureCapacity(int64_t target, bool exact) {
    if (getCapacity() >= target) {
        return;
    }

    if (!_canResize) {
        throw std::invalid_argument("cannot enlarge buffer: auto resize disabled");
    }

    if (target <= 0) {
        throw std::invalid_argument("invalid capacity");
    }

    int64_t newCapacity;
    const auto oldCapacity = getCapacity();

    if (exact) {
        newCapacity = target;
    } else {
        newCapacity = oldCapacity;

        while (newCapacity < target) {
            newCapacity = static_cast<int64_t>(newCapacity * 1.15f);
        }

        // Never waste more than 4KB
        newCapacity = std::min(newCapacity, target + 4096);
    }

    assert(newCapacity > oldCapacity);

    auto arr = make_array_dynamic<uint8_t>(newCapacity);
    const auto oldLength = getLength();

    if (oldLength > 0) {
        memcpy(arr->data(), _buffer->data(), oldLength);
    }

    if (newCapacity > oldLength) {
        memset(arr->data() + oldLength, 0, newCapacity - oldLength);
    }

    _buffer = arr;
}

int64_t MemoryStream::getLength() {
    return _length;
}

void MemoryStream::setLength(int64_t length) {
    if (!_canResize && length != getLength()) {
        throw std::runtime_error("cannot resize stream");
    }

    if (length < 0) {
        throw std::invalid_argument("invalid length");
    }

    const auto oldLength = getLength();

    if (length > oldLength) {
        ensureCapacity(length, false);
    } else if (length < oldLength) {
        auto arr = make_array_dynamic<uint8_t>(length);
        ArrayHelper::copyByValue(_buffer, 0, arr, 0, length);
        _buffer = arr;

        if (getPosition() > length) {
            setPosition(length, true);
        }
    }

    _length = length;
}

int64_t MemoryStream::getCapacity() const {
    return _buffer->size();
}

void MemoryStream::setCapacity(int64_t capacity) {
    setLength(capacity);
}

array_ptr<uint8_t> MemoryStream::toArray() {
    const auto length = getLength();

    auto arr = make_array_dynamic<uint8_t>(length);

    if (length > 0) {
        ArrayHelper::copyByValue(_buffer, 0, arr, 0, length);
    }

    return arr;
}

bool MemoryStream::canRead() const {
    return true;
}

bool MemoryStream::canWrite() const {
    return _writable;
}

bool MemoryStream::canSeek() const {
    return true;
}

array_ptr<uint8_t> MemoryStream::getBuffer() const {
    return _buffer;
}

void MemoryStream::writeTo(const shared_ptr<Stream> &stream) {
    stream->write(_buffer, 0, getLength());
}

void MemoryStream::setPosition(int64_t position, bool direct) {
    if (position < 0) {
        throw std::invalid_argument("invalid position");
    }

    if (direct) {
        _position = position;
        return;
    }

    const auto length = getLength();

    if (position < length) {
        _position = position;
    } else {
        if (length > 0) {
            ensureCapacity(position, false);
            _position = position;
        } else {
            _position = 0;
        }
    }
}
