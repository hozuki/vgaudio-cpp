#include "../../../common/io/BinaryReader.h"
#include "WaveFactChunk.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

WaveFactChunk::WaveFactChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader)
    : MyBase(reader), _sampleCount(0) {
    auto count = reader->readInt32();
    setSampleCount(count);
}

shared_ptr<WaveFactChunk> WaveFactChunk::parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader) {
    auto c = new WaveFactChunk(parser, reader);
    return shared_ptr<WaveFactChunk>(c);
}

int32_t WaveFactChunk::getSampleCount() const {
    return _sampleCount;
}

void WaveFactChunk::setSampleCount(int32_t sampleCount) {
    _sampleCount = sampleCount;
}
