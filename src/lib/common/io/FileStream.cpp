#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>

#include "../utilities/StrHelper.h"
#include "FileStream.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

static inline FILE *FP(void *fp) {
    return static_cast<FILE *>(fp);
}

static inline fstream *FS(void *fs) {
    return static_cast<fstream *>(fs);
}

static inline bool streq(const char *s1, const char *s2) {
    return strcmp(s1, s2) == 0;
}

static inline std::ios_base::openmode ModeStrToEnum(const char *mode) {
    using M = std::ios_base::openmode;

    if (streq(mode, "r")) {
        return std::ios_base::in;
    }
    if (streq(mode, "w")) {
        return std::ios_base::out | std::ios_base::trunc;
    }
    if (streq(mode, "a")) {
        return std::ios_base::out | std::ios_base::app | std::ios_base::ate;
    }

    if (streq(mode, "r+")) {
        return std::ios_base::in | std::ios_base::out;
    }
    if (streq(mode, "w+")) {
        return std::ios_base::in | std::ios_base::out | std::ios_base::trunc;
    }
    if (streq(mode, "a+")) {
        return std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate;
    }

    if (streq(mode, "rb")) {
        return std::ios_base::in | std::ios_base::binary;
    }
    if (streq(mode, "wb")) {
        return std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;
    }
    if (streq(mode, "ab")) {
        return std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary;
    }

    if (streq(mode, "r+b") || streq(mode, "rb+")) {
        return std::ios_base::in | std::ios_base::out | std::ios_base::binary;
    }
    if (streq(mode, "w+b") || streq(mode, "wb+")) {
        return std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;
    }
    if (streq(mode, "a+b") || streq(mode, "ab+")) {
        return std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary;
    }

    throw std::runtime_error("invalid file open mode");
}

#define pFS FS(_fp)
#define pFP FP(_fp)

FileStream::FileStream(const char *filePath, std::ios_base::openmode mode)
    : _mode(mode), _filePath(filePath) {
    initialize();
}

FileStream::FileStream(const std::string &filePath, std::ios_base::openmode mode)
    : MyClass(filePath.c_str(), mode) {
}

FileStream::FileStream(const char *filePath, const char *mode)
    : MyClass(filePath, ModeStrToEnum(mode)) {
}

FileStream::FileStream(const std::string &filePath, const char *mode)
    : MyClass(filePath.c_str(), mode) {
}

int32_t FileStream::read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (!canRead()) {
        throw std::runtime_error("not supported");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    const auto posBefore = _file.tellg();
    _file.read(static_cast<char *>(buffer) + offset, count);
    const auto posAfter = _file.tellg();
    return posAfter - posBefore;
}

int32_t FileStream::write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) {
    if (!canWrite()) {
        throw std::runtime_error("not supported");
    }

    validateBufferParameters(buffer, bufferSize, offset, count);

    const auto posBefore = _file.tellp();
    _file.write(static_cast<const char *>(buffer) + offset, count);
    const auto posAfter = _file.tellp();
    return posAfter - posBefore;
}

void FileStream::flush() {
    _file.flush();
}

void FileStream::seek(int64_t offset, SeekOrigin origin) {
    _file.seekg(offset, static_cast<std::ios_base::seekdir>(origin));
    _file.seekp(offset, static_cast<std::ios_base::seekdir>(origin));
}

int64_t FileStream::getPosition() {
    return _file.tellg();
}

void FileStream::setPosition(int64_t position) {
    seek(position, SeekOrigin::Begin);
}

int64_t FileStream::getLength() {
    if (canRead()) {
        const auto cur = _file.tellg();
        _file.seekg(0, std::ios_base::end);
        const auto tail = _file.tellg();
        if (tail != cur) {
            _file.seekg(cur);
        }
        return tail;
    } else if (canWrite()) {
        const auto cur = _file.tellp();
        _file.seekp(0, std::ios_base::end);
        const auto tail = _file.tellp();
        if (tail != cur) {
            _file.seekp(cur);
        }
        return tail;
    } else {
        throw std::runtime_error("reading/writing are both not supported");
    }
}

void FileStream::setLength(int64_t length) {
    if (!canWrite()) {
        throw std::runtime_error("cannot resize a file stream that cannot be written");
    }

    assert(length >= 0);

    if (length == getLength()) {
        return;
    }


}

bool FileStream::canRead() const {
    return (_mode & std::ios_base::in) != 0;
}

bool FileStream::canWrite() const {
    return (_mode & std::ios_base::out) != 0;
}

bool FileStream::canSeek() const {
    return true;
}

const std::string &FileStream::getFilePath() const {
    return _filePath;
}

std::ios_base::openmode FileStream::getOpenMode() const {
    return _mode;
}

void FileStream::initialize() {
    _file = fstream(getFilePath(), getOpenMode());
}
