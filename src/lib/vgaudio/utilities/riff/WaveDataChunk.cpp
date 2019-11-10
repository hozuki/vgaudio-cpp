#include "../../../common/io/BinaryReader.h"
#include "RiffParser.h"
#include "WaveDataChunk.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

WaveDataChunk::WaveDataChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader)
    : MyBase(reader) {
    if (parser->hasReadDataChunk()) {
        const auto subChunkSize = getSubChunkSize();
        _data = reader->readBytes(subChunkSize);
    }
}

shared_ptr<WaveDataChunk> WaveDataChunk::parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader) {
    auto c = new WaveDataChunk(parser, reader);
    return shared_ptr<WaveDataChunk>(c);
}

array_ptr<uint8_t> WaveDataChunk::getData() const {
    return _data;
}

void WaveDataChunk::setData(const array_ptr<uint8_t> &data) {
    _data = data;
}
