#include <algorithm>

#include "../../../common/io/BinaryReader.h"
#include "RiffParser.h"
#include "WaveFormatTags.h"
#include "WaveFormatExtensible.h"
#include "WaveFmtChunk.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

WaveFmtChunk::WaveFmtChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader)
    : MyBase(reader) {
    auto formatTag = reader->readUInt16();
    auto channelCount = reader->readInt16();
    auto sampleRate = reader->readInt32();
    auto bps = reader->readInt32();
    auto blockAlign = reader->readInt16();
    auto bitsPerSample = reader->readInt16();

    shared_ptr<WaveFormatExtensible> ext = nullptr;
    const auto extParser = parser->getFormatExtensibleParser();

    if (formatTag == WaveFormatTags::Extensible && extParser != nullptr) {
        auto stream = reader->getBaseStream();
        int64_t startOffset = stream->getPosition() + 2;
        ext = extParser(parser, reader);

        int64_t endOffset = startOffset + ext->getSize();
        int32_t remainingBytes = std::max<int64_t>(endOffset - stream->getPosition(), 0);
        ext->setExtra(reader->readBytes(remainingBytes));
    }

    setFormatTag(formatTag);
    setChannelCount(channelCount);
    setSampleRate(sampleRate);
    setAverageBytesPerSecond(bps);
    setBlockAlign(blockAlign);
    setBitsPerSample(bitsPerSample);
}

shared_ptr<WaveFmtChunk> WaveFmtChunk::parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader) {
    auto c = new WaveFmtChunk(parser, reader);
    return shared_ptr<WaveFmtChunk>(c);
}

int32_t WaveFmtChunk::getFormatTag() const {
    return _formatTag;
}

void WaveFmtChunk::setFormatTag(int32_t tag) {
    _formatTag = tag;
}

int32_t WaveFmtChunk::getChannelCount() const {
    return _channelCount;
}

void WaveFmtChunk::setChannelCount(int32_t channelCount) {
    _channelCount = channelCount;
}

int32_t WaveFmtChunk::getSampleRate() const {
    return _sampleRate;
}

void WaveFmtChunk::setSampleRate(int32_t sampleRate) {
    _sampleRate = sampleRate;
}

int32_t WaveFmtChunk::getAverageBytesPerSecond() const {
    return _averageBytesPerSecond;
}

void WaveFmtChunk::setAverageBytesPerSecond(int32_t bytesPerSecond) {
    _averageBytesPerSecond = bytesPerSecond;
}

int32_t WaveFmtChunk::getBlockAlign() const {
    return _blockAlign;
}

void WaveFmtChunk::setBlockAlign(int32_t align) {
    _blockAlign = align;
}

int32_t WaveFmtChunk::getBitsPerSample() const {
    return _bitsPerSample;
}

void WaveFmtChunk::setBitsPerSample(int32_t bitsPerSample) {
    _bitsPerSample = bitsPerSample;
}

shared_ptr<WaveFormatExtensible> WaveFmtChunk::getExtensible() const {
    return _extensible;
}

void WaveFmtChunk::setExtensible(const shared_ptr<WaveFormatExtensible> &extensible) {
    _extensible = extensible;
}
