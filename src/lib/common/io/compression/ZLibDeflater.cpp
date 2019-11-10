#include <cassert>
#include <stdexcept>
#include <cstring>
#include <zlib.h>

#include "../../utilities/defer.h"
#include "private/zlib_debug.h"
#include "private/zlib_settings.h"
#include "ZLibDeflater.h"

using namespace std;
using namespace common_lib::io;
using namespace common_lib::io::compression;

ZLibDeflater::ZLibDeflater(int32_t level)
    : _level(level), _inputBuffer(nullptr), _inputBufferSize(0) {
    _z = new z_stream();
    initialize();
}

ZLibDeflater::~ZLibDeflater() {
    if (_z) {
        deflateEnd(_z);
    }

    delete _z;

    disposeInputBuffer();
}

void ZLibDeflater::initialize() {
    invokeDeflateInit(_level, WINDOW_BITS_DEFLATE, DEFAULT_MEM_LEVEL, Z_DEFAULT_STRATEGY);
}

bool ZLibDeflater::needsInput() const {
    return _z->avail_in == 0;
}

void ZLibDeflater::setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) {
    assert(needsInput());
    assert(inputBuffer != nullptr);
    assert(offset >= 0 && count > 0 && count + offset <= inputBufferSize);
    assert(!isInputBufferCreated());

    recreateInputBuffer(inputBufferSize);
    memcpy(_inputBuffer, inputBuffer, inputBufferSize);

    _z->next_in = _inputBuffer + offset;
    _z->avail_in = count;
}

int32_t ZLibDeflater::getDeflateOutput(void *outputBuffer, size_t outputBufferSize) {
    assert(outputBuffer != nullptr);
    assert(!needsInput());
    assert(isInputBufferCreated());

    auto_defer([this]() {
        if (this->needsInput() && this->isInputBufferCreated()) {
            this->disposeInputBuffer();
        }
    });

    int32_t bytesRead = 0;
    readDeflateOutput(outputBuffer, outputBufferSize, Z_NO_FLUSH, &bytesRead);

    assert(0 <= bytesRead && bytesRead <= outputBufferSize);

    return bytesRead;
}

bool ZLibDeflater::finish(void *outputBuffer, size_t outputBufferSize, int32_t *bytesRead) {
    assert(outputBuffer != nullptr);
    assert(needsInput());
    assert(!isInputBufferCreated());

    auto err = readDeflateOutput(outputBuffer, outputBufferSize, Z_FINISH, bytesRead);
    return err == Z_STREAM_END;
}

z_error_code ZLibDeflater::readDeflateOutput(void *outputBuffer, size_t outputBufferSize, z_flush_code flushCode, int32_t *bytesRead) {
    auto p = static_cast<uint8_t *>(outputBuffer);
    _z->next_out = p;
    _z->avail_out = outputBufferSize;

    auto err = invokeDeflate(flushCode);

    if (bytesRead != nullptr) {
        *bytesRead = outputBufferSize - _z->avail_out;
    }

    return err;
}

void ZLibDeflater::invokeDeflateInit(int32_t compressionLevel, int32_t windowBits, int32_t memoryLevel, int32_t strategy) {
    _z->zalloc = nullptr;
    _z->zfree = nullptr;
    _z->opaque = nullptr;

    int ret = deflateInit2(_z, compressionLevel, Z_DEFLATED, windowBits, memoryLevel, strategy);
    ThrowCommonZLibError("deflate init", ret);
}

z_error_code ZLibDeflater::invokeDeflate(z_flush_code flushCode) {
    auto ret = deflate(_z, flushCode);

    switch (ret) {
        case Z_OK:
        case Z_STREAM_END:
            return ret;
        case Z_BUF_ERROR:
            return ret;
        default:
            ThrowCommonZLibError("deflate invoke", ret);
            return 0;
    }
}

void ZLibDeflater::recreateInputBuffer(size_t size) {
    assert(!isInputBufferCreated());
    _inputBuffer = new uint8_t[size];
    _inputBufferSize = size;
    memset(_inputBuffer, 0, size);
}

void ZLibDeflater::disposeInputBuffer() {
    delete[] _inputBuffer;
    _inputBuffer = nullptr;
}

bool ZLibDeflater::isInputBufferCreated() const {
    return _inputBuffer != nullptr;
}
