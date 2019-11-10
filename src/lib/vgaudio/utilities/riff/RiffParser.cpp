#include <algorithm>

#include "../../../common/utilities/macros.h"
#include "../../../common/io/BinaryReader.h"
#include "../../../common/utilities/EndianHelper.h"
#include "RiffChunk.h"
#include "WaveDataChunk.h"
#include "WaveFactChunk.h"
#include "WaveFmtChunk.h"
#include "WaveSmplChunk.h"
#include "WaveFormatExtensible.h"
#include "RiffParser.h"

using namespace std;
using namespace common_lib::io;
using namespace common_lib::utilities;
using namespace vgaudio::utilities::riff;

RiffParser::RiffParser()
    : _readDataChunk(true) {
    registerSubChunk("data", WaveDataChunk::parse);
    registerSubChunk("fact", WaveFactChunk::parse);
    registerSubChunk("fmt ", WaveFmtChunk::parse);
    registerSubChunk("smpl", WaveSmplChunk::parse);

    setFormatExtensibleParser(WaveFormatExtensible::parse);
}

shared_ptr<RiffChunk> RiffParser::getRiffChunk() const {
    return _riffChunk;
}

void RiffParser::setRiffChunk(const shared_ptr<RiffChunk> &chunk) {
    _riffChunk = chunk;
}

bool RiffParser::hasReadDataChunk() const {
    return _readDataChunk;
}

void RiffParser::setReadDataChunk(bool read) {
    _readDataChunk = read;
}

function<shared_ptr<WaveFormatExtensible>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> RiffParser::getFormatExtensibleParser() const {
    return _formatExtensibleParser;
}

void RiffParser::setFormatExtensibleParser(const function<shared_ptr<WaveFormatExtensible>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> &p) {
    _formatExtensibleParser = p;
}

void RiffParser::registerSubChunk(const string &id, const function<shared_ptr<RiffSubChunk>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> &subChunkReader) {
    if (id.length() != 4) {
        throw std::invalid_argument("sub chunk ID must be 4 characters long");
    }

    _registeredSubChunks[id] = subChunkReader;
}

shared_ptr<RiffSubChunk> RiffParser::getSubChunk(const string &id) {
    if (_subChunks.find(id) == _subChunks.end()) {
        return nullptr;
    } else {
        return _subChunks[id];
    }
}

vector<shared_ptr<RiffSubChunk>> RiffParser::getAllSubChunks() const {
    vector<shared_ptr<RiffSubChunk>> result;

    for (const auto &kv : _subChunks) {
        result.push_back(kv.second);
    }

    return result;
}

void RiffParser::parseRiff(const shared_ptr<Stream> &file) {
    auto reader = EndianHelper::createBinaryReader(file, Endianess::LittleEndian);

    auto riffChunk = RiffChunk::parse(reader);
    setRiffChunk(riffChunk);

    _subChunks.clear();

    auto baseStream = reader->getBaseStream();

    const int64_t startOffset = baseStream->getPosition() - 4;
    const int64_t endOffset = startOffset + riffChunk->getSize();

    while (baseStream->getPosition() + 8 < endOffset) {
        auto subChunk = parseSubChunk(reader);
        _subChunks[subChunk->getSubChunkId()] = subChunk;
    }
}

shared_ptr<RiffSubChunk> RiffParser::parseSubChunk(const shared_ptr<BinaryReader> &reader) {
    auto stream = reader->getBaseStream();

    auto id = reader->readFixedString(4);
    stream->setPosition(stream->getPosition() - 4);
    int64_t startOffset = stream->getPosition() + 8;

    shared_ptr<RiffSubChunk> subChunk;

    if (_registeredSubChunks.find(id) != _registeredSubChunks.end()) {
        auto parserFunc = _registeredSubChunks[id];
        auto thiz = shared_ptr<MyClass>(this, DONT_DELETE(MyClass));
        subChunk = parserFunc(thiz, reader);
    } else {
        subChunk = make_shared<RiffSubChunk>(reader);
    }

    int64_t endOffset = startOffset + subChunk->getSubChunkSize();
    int32_t remainingBytes = std::max<int64_t>(endOffset - stream->getPosition(), 0);

    auto extra = reader->readBytes(remainingBytes);
    subChunk->setExtra(extra);

    // Sub chunks are 2-byte aligned
    stream->setPosition(endOffset + (endOffset & 0x1));

    return subChunk;
}
