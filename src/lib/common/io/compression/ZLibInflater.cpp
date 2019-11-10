#include <cassert>
#include <stdexcept>
#include <cstring>
#include <zlib.h>

#include "../../utilities/defer.h"
#include "private/zlib_debug.h"
#include "ZLibInflater.h"

using namespace std;
using namespace common_lib::io::compression;

ZLibInflater::ZLibInflater(int32_t windowBits)
    : _z(nullptr), _inputBuffer(nullptr), _inputBufferSize(0), _isFinished(false) {
    _z = new z_stream();
    initialize(windowBits);
}

ZLibInflater::~ZLibInflater() {
    if (_z) {
        inflateEnd(_z);
    }

    delete _z;

    disposeInputBuffer();
}

int32_t ZLibInflater::getAvailableOutput() {
    return _z->avail_out;
}

bool ZLibInflater::needsInput() const {
    return _z->avail_in == 0;
}

void ZLibInflater::setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) {
    if (isFinished()) {
        return;
    }

    assert(needsInput());
    assert(inputBuffer != nullptr);
    assert(offset >= 0 && count > 0 && count + offset <= inputBufferSize);
    assert(!isInputBufferCreated());

    recreateInputBuffer(count);
    memcpy(_inputBuffer, static_cast<const uint8_t *>(inputBuffer) + offset, count);

    _z->next_in = _inputBuffer;
    _z->avail_in = count;
}

int32_t ZLibInflater::getInflateOutput(void *outputBuffer, size_t outputBufferSize, int32_t offset, int32_t count) {
    if (count == 0) {
        return 0;
    }

    if (isFinished()) {
        return 0;
    }

    int32_t bytesRead = 0;
    auto err = readInflateOutput(outputBuffer, outputBufferSize, offset, count, Z_NO_FLUSH, &bytesRead);

    auto_defer([this]() {
        if (this->needsInput() && this->isInputBufferCreated()) {
            this->disposeInputBuffer();
        }
    });

    if (err == Z_STREAM_END) {
        _isFinished = true;
    }

    return bytesRead;
}

bool ZLibInflater::isFinished() const {
    return _isFinished;
}

void ZLibInflater::initialize(int32_t windowBits) {
    invokeInflateInit(windowBits);
}

void ZLibInflater::invokeInflateInit(int32_t windowBits) {
    _z->zalloc = nullptr;
    _z->zfree = nullptr;
    _z->opaque = nullptr;
    _z->next_in = nullptr;
    _z->avail_in = 0;

    int ret = inflateInit2(_z, windowBits);
    ThrowCommonZLibError("inflate init", ret);
}

z_error_code ZLibInflater::invokeInflate(z_flush_code flushCode) noexcept(false) {
    auto ret = inflate(_z, flushCode);

    switch (ret) {
        case Z_OK:
        case Z_STREAM_END:
            return ret;
        case Z_BUF_ERROR:
            return ret;
        default:
            ThrowCommonZLibError("inflate invoke", ret);
            return 0;
    }
}

void ZLibInflater::recreateInputBuffer(size_t size) {
    assert(!isInputBufferCreated());
    _inputBuffer = new uint8_t[size];
    _inputBufferSize = size;
    memset(_inputBuffer, 0, size);
}

void ZLibInflater::disposeInputBuffer() {
    delete[] _inputBuffer;
    _inputBuffer = nullptr;
}

bool ZLibInflater::isInputBufferCreated() const {
    return _inputBuffer != nullptr;
}

z_error_code ZLibInflater::readInflateOutput(void *outputBuffer, size_t outputBufferSize, int32_t offset, int32_t count, z_flush_code flushCode, int32_t *bytesRead) {
    _z->next_out = static_cast<uint8_t *>(outputBuffer) + offset;
    _z->avail_out = count;

    auto ret = invokeInflate(flushCode);

    int32_t read = count - _z->avail_out;

    if (bytesRead != nullptr) {
        *bytesRead = read;
    }

    return ret;
}
