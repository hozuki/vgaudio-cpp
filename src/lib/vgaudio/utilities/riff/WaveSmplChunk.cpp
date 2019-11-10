#include "../../../common/io/BinaryReader.h"
#include "WaveSmplChunk.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities::riff;

WaveSmplChunk::SampleLoop::SampleLoop()
    : cuePointId(0), type(0), start(0), end(0), fraction(0), playCount(0) {
}

WaveSmplChunk::WaveSmplChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader)
    : MyBase(reader) {
    auto manufacturer = reader->readInt32();
    auto product = reader->readInt32();
    auto samplePeriod = reader->readInt32();
    auto midiUnityNote = reader->readInt32();
    auto midiPitchFraction = reader->readInt32();
    auto smpteFormat = reader->readInt32();
    auto smpteOffset = reader->readInt32();
    auto sampleLoopCount = reader->readInt32();
    auto samplerData = reader->readInt32();

    auto loops = make_array_dynamic<WaveSmplChunk::SampleLoop>(sampleLoopCount);

    for (auto i = 0; i < sampleLoopCount; i += 1) {
        WaveSmplChunk::SampleLoop sl;
        sl.cuePointId = reader->readInt32();
        sl.type = reader->readInt32();
        sl.start = reader->readInt32();
        sl.end = reader->readInt32();
        sl.fraction = reader->readInt32();
        sl.playCount = reader->readInt32();
        (*loops)[i] = sl;
    }

    setManufacturer(manufacturer);
    setProduct(product);
    setSamplePeriod(samplePeriod);
    setMidiUnityNote(midiUnityNote);
    setMidiPitchFraction(midiPitchFraction);
    setSmpteFormat(smpteFormat);
    setSmpteOffset(smpteOffset);
    setSamplerData(samplerData);
    setLoops(loops);
}

shared_ptr<WaveSmplChunk> WaveSmplChunk::parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader) {
    auto c = new WaveSmplChunk(parser, reader);
    return shared_ptr<WaveSmplChunk>(c);
}

int32_t WaveSmplChunk::getManufacturer() const {
    return _manufacturer;
}

void WaveSmplChunk::setManufacturer(int32_t manufacturer) {
    _manufacturer = manufacturer;
}

int32_t WaveSmplChunk::getProduct() const {
    return _product;
}

void WaveSmplChunk::setProduct(int32_t product) {
    _product = product;
}

int32_t WaveSmplChunk::getSamplePeriod() const {
    return _samplePeriod;
}

void WaveSmplChunk::setSamplePeriod(int32_t period) {
    _samplePeriod = period;
}

int32_t WaveSmplChunk::getMidiUnityNote() const {
    return _midiUnityNote;
}

void WaveSmplChunk::setMidiUnityNote(int32_t note) {
    _midiUnityNote = note;
}

int32_t WaveSmplChunk::getMidiPitchFraction() const {
    return _midiPitchFraction;
}

void WaveSmplChunk::setMidiPitchFraction(int32_t fraction) {
    _midiPitchFraction = fraction;
}

int32_t WaveSmplChunk::getSmpteFormat() const {
    return _smpteFormat;
}

void WaveSmplChunk::setSmpteFormat(int32_t format) {
    _smpteFormat = format;
}

int32_t WaveSmplChunk::getSmpteOffset() const {
    return _smpteOffset;
}

void WaveSmplChunk::setSmpteOffset(int32_t offset) {
    _smpteOffset = offset;
}

int32_t WaveSmplChunk::getSampleLoopCount() const {
    if (_loops == nullptr) {
        return 0;
    } else {
        return _loops->size();
    }
}

int32_t WaveSmplChunk::getSamplerData() const {
    return _samplerData;
}

void WaveSmplChunk::setSamplerData(int32_t data) {
    _samplerData = data;
}

array_ptr<WaveSmplChunk::SampleLoop> WaveSmplChunk::getLoops() const {
    return _loops;
}

void WaveSmplChunk::setLoops(const array_ptr<WaveSmplChunk::SampleLoop> &loops) {
    _loops = loops;
}
