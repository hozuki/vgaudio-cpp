#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/Guid.h"
#include "../../../common/utilities/EndianHelper.h"
#include "../../../common/io/BinaryWriter.h"
#include "../../formats/pcm8/UnsignedPcm8Format.h"
#include "../../utilities/Interleave.h"
#include "../../utilities/riff/WaveFormatTags.h"
#include "../../utilities/riff/MediaSubtypes.h"
#include "WaveWriter.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::containers::wave;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm8;
using namespace vgaudio::utilities;
using namespace vgaudio::utilities::riff;

static int32_t GetChannelMask(int32_t channelCount) {
    switch (channelCount) {
        case 4:
            return 0x0033;
        case 5:
            return 0x0133;
        case 6:
            return 0x0633;
        case 7:
            return 0x01f3;
        case 8:
            return 0x06f3;
        default:
            return (1 << channelCount) - 1;
    }
}

static array_ptr<uint8_t> GuidToByteArray(const Guid &guid) {
    assert(sizeof(Guid) == 16);

    const auto data = new uint8_t[sizeof(Guid)];
    memcpy(data, &guid, sizeof(Guid));

    return make_array_dynamic<uint8_t>(data, sizeof(Guid));
}

WaveCodec WaveWriter::getCodec() const {
    return getConfiguration()->codec;
}

int32_t WaveWriter::getChannelCount() const {
    return _audioFormat->getChannelCount();
}

int32_t WaveWriter::getSampleCount() const {
    return _audioFormat->getSampleCount();
}

int32_t WaveWriter::getSampleRate() const {
    return _audioFormat->getSampleRate();
}

bool WaveWriter::isLooping() const {
    return _audioFormat->isLooping();
}

int32_t WaveWriter::getLoopStart() const {
    return _audioFormat->getLoopStart();
}

int32_t WaveWriter::getLoopEnd() const {
    return _audioFormat->getLoopEnd();
}

int32_t WaveWriter::getRiffChunkSize() const {
    return 4 + 8 + getFmtChunkSize() + 8 + getDataChunkSize() + (isLooping() ? 8 + getSmplChunkSize() : 0);
}

int32_t WaveWriter::getFmtChunkSize() const {
    return getChannelCount() > 2 ? 40 : 16;
}

int32_t WaveWriter::getDataChunkSize() const {
    return getChannelCount() * getSampleCount() * getBytesPerSample();
}

int32_t WaveWriter::getSmplChunkSize() const {
    return 0x3c;
}

int32_t WaveWriter::getBitDepth() const {
    const auto codec = getCodec();

    switch (codec) {
        case WaveCodec::Pcm16Bit:
            return 16;
        case WaveCodec::Pcm8Bit:
            return 8;
        default:
            throw std::out_of_range("unknown wave codec");
    }
}

int32_t WaveWriter::getBytesPerSample() const {
    return IntHelper::divideByRoundUp(getBitDepth(), 8);
}

int32_t WaveWriter::getBytesPerSecond() const {
    return getSampleRate() * getBytesPerSample() * getChannelCount();
}

int32_t WaveWriter::getBlockAlign() const {
    return getBytesPerSample() * getChannelCount();
}

void WaveWriter::setupWriter(const shared_ptr<AudioData> &audio) {
    const auto parameters = make_shared<CodecParameters>();
    parameters->progress = getConfiguration()->progress;

    const auto codec = getCodec();

    switch (codec) {
        case WaveCodec::Pcm16Bit:
            _pcm16 = AudioData::getFormat<Pcm16Format>(audio, parameters);
            _audioFormat = _pcm16;
            break;
        case WaveCodec::Pcm8Bit:
            _pcm8 = AudioData::getFormat<UnsignedPcm8Format>(audio, parameters);
            _audioFormat = _pcm8;
            break;
        default:
            throw std::out_of_range("unknown wave codec");
    }
}

void WaveWriter::writeStream(const shared_ptr<Stream> &stream) {
    auto writer = EndianHelper::createBinaryWriter(stream, Endianess::LittleEndian);

    writeRiffHeader(writer);
    writeFmtChunk(writer);
    writeDataChunk(writer);

    if (isLooping()) {
        writeSmplChunk(writer);
    }
}

int32_t WaveWriter::getFileSize() {
    return 8 + getRiffChunkSize();
}

void WaveWriter::writeRiffHeader(const shared_ptr<BinaryWriter> &writer) {
    writer->writeFixedString("RIFF", 4);
    writer->write(getRiffChunkSize());
    writer->writeFixedString("WAVE", 4);
}

void WaveWriter::writeFmtChunk(const shared_ptr<BinaryWriter> &writer) {
    const auto &baseStream = writer->getBaseStream();
    baseStream->setPosition(baseStream->getPosition() + (baseStream->getPosition() & 1));

    const auto channelCount = getChannelCount();

    writer->writeFixedString("fmt ", 4);
    writer->write(getFmtChunkSize());
    writer->write(static_cast<int16_t>(channelCount > 2 ? WaveFormatTags::Extensible : WaveFormatTags::Pcm));
    writer->write(static_cast<int16_t>(channelCount));
    writer->write(getSampleRate());
    writer->write(getBytesPerSecond());
    writer->write(static_cast<int16_t>(getBlockAlign()));
    writer->write(static_cast<int16_t>(getBitDepth()));

    if (channelCount > 2) {
        writer->write(static_cast<int16_t>(22));
        writer->write(static_cast<int16_t>(getBitDepth()));
        writer->write(GetChannelMask(channelCount));

        auto guidData = GuidToByteArray(MediaSubtypes::Pcm);
        writer->write(guidData, guidData->size());
    }
}

void WaveWriter::writeDataChunk(const shared_ptr<BinaryWriter> &writer) {
    const auto &baseStream = writer->getBaseStream();
    baseStream->setPosition(baseStream->getPosition() + (baseStream->getPosition() & 1));

    writer->writeFixedString("data", 4);
    writer->write(getDataChunkSize());

    const auto codec = getCodec();

    switch (codec) {
        case WaveCodec::Pcm16Bit: {
            auto audioData = Interleave::shortToInterleavedByte(_pcm16->getChannels());
            writer->write(audioData, audioData->size());
            break;
        }
        case WaveCodec::Pcm8Bit:
            Interleave::interleave(_pcm8->getChannels(), baseStream, getBytesPerSample());
            break;
        default:
            throw std::out_of_range("unknown wave codec");
    }
}

void WaveWriter::writeSmplChunk(const shared_ptr<BinaryWriter> &writer) {
    const auto &baseStream = writer->getBaseStream();
    baseStream->setPosition(baseStream->getPosition() + (baseStream->getPosition() & 1));

    writer->writeFixedString("smpl", 4);
    writer->write(getSmplChunkSize());

    for (auto i = 0; i < 7; i += 1) {
        writer->write(static_cast<int32_t>(0));
    }
    writer->write(static_cast<int32_t>(1));
    for (auto i = 0; i < 3; i += 1) {
        writer->write(static_cast<int32_t>(0));
    }
    writer->write(getLoopStart());
    writer->write(getLoopEnd());
    for (auto i = 0; i < 2; i += 1) {
        writer->write(static_cast<int32_t>(0));
    }
}
