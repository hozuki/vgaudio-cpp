#include "../../../common/io/BinaryReader.h"
#include "RiffSubChunk.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

RiffSubChunk::RiffSubChunk(const shared_ptr<BinaryReader> &reader) {
    _subChunkId = reader->readFixedString(4);
    _subChunkSize = reader->readInt32();
}

std::string RiffSubChunk::getSubChunkId() const {
    return _subChunkId;
}

void RiffSubChunk::setSubChunkId(const std::string &id) {
    _subChunkId = id;
}

int32_t RiffSubChunk::getSubChunkSize() const {
    return _subChunkSize;
}

void RiffSubChunk::setSubChunkSize(int32_t size) {
    _subChunkSize = size;
}

array_ptr<uint8_t> RiffSubChunk::getExtra() const {
    return _extra;
}

void RiffSubChunk::setExtra(const array_ptr<uint8_t> &extra) {
    _extra = extra;
}
