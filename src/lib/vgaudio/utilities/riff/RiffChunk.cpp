#include <stdexcept>

#include "../../../common/io/BinaryReader.h"
#include "RiffChunk.h"

using namespace std;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

RiffChunk::RiffChunk()
    : _size(0) {
}

shared_ptr<RiffChunk> RiffChunk::parse(const shared_ptr<BinaryReader> &reader) {
    auto chunkId = reader->readFixedString(4);

    if (chunkId != "RIFF") {
        throw std::runtime_error("invalid RIFF file");
    }

    auto size = reader->readUInt32();
    auto type = reader->readFixedString(4);

    auto chunk = new RiffChunk();

    chunk->setChunkId(chunkId);
    chunk->setSize(size);
    chunk->setType(type);

    return shared_ptr<RiffChunk>(chunk);
}

string RiffChunk::getChunkId() const {
    return _chunkId;
}

void RiffChunk::setChunkId(const string &id) {
    _chunkId = id;
}

int32_t RiffChunk::getSize() const {
    return _size;
}

void RiffChunk::setSize(int32_t size) {
    _size = size;
}

string RiffChunk::getType() const {
    return _type;
}

void RiffChunk::setType(const string &type) {
    _type = type;
}
