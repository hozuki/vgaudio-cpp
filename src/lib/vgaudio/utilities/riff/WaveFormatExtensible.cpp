#include "../../../common/io/BinaryReader.h"
#include "WaveFormatExtensible.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

WaveFormatExtensible::WaveFormatExtensible(const shared_ptr<BinaryReader> &reader) {
    auto size = reader->readInt16();
    auto validBitsPerSample = reader->readInt16();
    auto channelMask = reader->readUInt32();
    const auto guidBytes = reader->readBytes(16);
    auto subFormat = Guid(guidBytes->data());

    setSize(size);
    setValidBitsPerSample(validBitsPerSample);
    setChannelMask(channelMask);
    setSubFormat(subFormat);
}

shared_ptr<WaveFormatExtensible> WaveFormatExtensible::parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader) {
    auto e = new WaveFormatExtensible(reader);
    return shared_ptr<WaveFormatExtensible>(e);
}

int32_t WaveFormatExtensible::getSize() const {
    return _size;
}

void WaveFormatExtensible::setSize(int32_t size) {
    _size = size;
}

int32_t WaveFormatExtensible::getValidBitsPerSample() const {
    return _validBitsPerSample;
}

void WaveFormatExtensible::setValidBitsPerSample(int32_t bits) {
    _validBitsPerSample = bits;
}

int32_t WaveFormatExtensible::getSamplesPerBlock() const {
    return getValidBitsPerSample();
}

void WaveFormatExtensible::setSamplesPerBlock(int32_t samples) {
    setValidBitsPerSample(samples);
}

uint32_t WaveFormatExtensible::getChannelMask() const {
    return _channelMask;
}

void WaveFormatExtensible::setChannelMask(uint32_t mask) {
    _channelMask = mask;
}

Guid WaveFormatExtensible::getSubFormat() const {
    return _subFormat;
}

void WaveFormatExtensible::setSubFormat(const Guid &format) {
    _subFormat = format;
}

array_ptr<uint8_t> WaveFormatExtensible::getExtra() const {
    return _extra;
}

void WaveFormatExtensible::setExtra(const array_ptr<uint8_t> &extra) {
    _extra = extra;
}
