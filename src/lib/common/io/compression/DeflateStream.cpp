#include <cassert>

#include "IDeflater.h"
#include "ZLibDeflater.h"
#include "IInflater.h"
#include "ZLibInflater.h"
#include "private/zlib_settings.h"
#include "DeflateStream.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace common_lib::io::compression;

constexpr size_t DefaultBufferSize = 8192;

DeflateStream::DeflateStream(const shared_ptr<Stream> &stream, CompressionMode mode)
    : MyClass(stream, mode, -1) {
}

DeflateStream::DeflateStream(const shared_ptr<Stream> &stream, CompressionMode mode, int32_t level)
    : _baseStream(stream), _mode(mode), _level(level), _wroteBytes(false),
      _buffer(nullptr), _bufferSize(0),
      _deflater(nullptr), _inflater(nullptr) {
    initialize();
}

DeflateStream::~DeflateStream() {
    purgeBuffers();

    delete _deflater;
    delete _inflater;
    delete[] _buffer;
}

void DeflateStream::initialize() {
    switch (getCompressionMode()) {
        case CompressionMode::Decompress:
            _inflater = new ZLibInflater(WINDOW_BITS_DEFLATE);
            break;
        case CompressionMode::Compress:
            _deflater = new ZLibDeflater(getCompressionLevel());
            break;
        default:
            throw std::out_of_range("invalid enum value");
    }

    _buffer = new uint8_t[DefaultBufferSize];
    _bufferSize = DefaultBufferSize;
    memset(_buffer, 0, _bufferSize);
}

shared_ptr<Stream> DeflateStream::getBaseStream() const {
    return _baseStream;
}

CompressionMode DeflateStream::getCompressionMode() const {
    return _mode;
}

int32_t DeflateStream::getCompressionLevel() const {
    return _level;
}

bool DeflateStream::canRead() const {
    return getCompressionMode() == CompressionMode::Decompress && _baseStream->canRead();
}

bool DeflateStream::canWrite() const {
    return getCompressionMode() == CompressionMode::Compress && _baseStream->canWrite();
}

int32_t DeflateStream::read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (!canRead()) {
        throw std::runtime_error("invalid operation");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    if (count == 0) {
        return 0;
    }

    assert(_inflater != nullptr);

    int32_t bytesRead = 0;
    int32_t currentOffset = offset;
    int32_t remainingCount = count;

    while (true) {
        // The first time when it is called, set internal state to needsInput() = true,
        // and consequently flows to setInput().
        // In other words, the first time we call IInflater::inflate(), it only resets its state,
        // and does nothing else (no actual inflating).
        bytesRead = _inflater->getInflateOutput(buffer, bufferSize, currentOffset, remainingCount);
        currentOffset += bytesRead;
        remainingCount -= bytesRead;

        if (remainingCount == 0) {
            break;
        }

        if (_inflater->isFinished()) {
            break;
        }

        assert(_inflater->needsInput());

        const auto bytes = _baseStream->read(_buffer, _bufferSize, 0, _bufferSize);

        if (bytes == 0) {
            break;
        }

        _inflater->setInput(_buffer, _bufferSize, 0, bytes);
    }

    return count - remainingCount;
}

int32_t DeflateStream::write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (!canWrite()) {
        throw std::runtime_error("invalid operation");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    if (count == 0) {
        return 0;
    }

    assert(_deflater != nullptr);

    _wroteBytes = true;

    auto written = writeDeflaterOutput();

    _deflater->setInput(buffer, bufferSize, offset, count);
    written += writeDeflaterOutput();

    return written;
}

void DeflateStream::flush() {
    // Do nothing
}

void DeflateStream::purgeBuffers() {
    if (getCompressionMode() != CompressionMode::Compress) {
        return;
    }

    assert(_deflater != nullptr);

    if (_wroteBytes) {
        writeDeflaterOutput();

        bool finished;

        do {
            int32_t compressedBytes;
            finished = _deflater->finish(_buffer, _bufferSize, &compressedBytes);

            if (compressedBytes > 0) {
                _baseStream->write(_buffer, _bufferSize, 0, compressedBytes);
            }
        } while (!finished);
    }
}

int32_t DeflateStream::writeDeflaterOutput() {
    assert(_deflater != nullptr);

    int32_t written = 0;

    while (!_deflater->needsInput()) {
        auto compressedBytes = _deflater->getDeflateOutput(_buffer, _bufferSize);

        if (compressedBytes > 0) {
            written += _baseStream->write(_buffer, _bufferSize, 0, compressedBytes);
        }
    }

    return written;
}
