#include <cinttypes>

#include "../../formats/pcm16/Pcm16FormatBuilder.h"
#include "../../formats/pcm8/Pcm8FormatBuilder.h"
#include "../../utilities/riff/RiffParser.h"
#include "../../utilities/riff/RiffChunk.h"
#include "../../utilities/riff/WaveFmtChunk.h"
#include "../../utilities/riff/WaveSmplChunk.h"
#include "../../utilities/riff/WaveDataChunk.h"
#include "../../utilities/riff/WaveFormatExtensible.h"
#include "../../utilities/riff/WaveFormatTags.h"
#include "../../utilities/riff/MediaSubtypes.h"
#include "../../utilities/Interleave.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/StrHelper.h"
#include "WaveReader.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::containers;
using namespace vgaudio::containers::wave;
using namespace vgaudio::utilities;
using namespace vgaudio::utilities::riff;
using namespace vgaudio::formats::pcm16;
using namespace vgaudio::formats::pcm8;

shared_ptr<WaveStructure> WaveReader::readFile(const shared_ptr<Stream> &stream, bool readAudioData) {
    auto structure = make_shared<WaveStructure>();
    auto parser = make_shared<RiffParser>();

    parser->setReadDataChunk(readAudioData);
    parser->parseRiff(stream);

    validateWaveFile(parser);

    auto fmt = RiffParser::getSubChunk<WaveFmtChunk>(parser, "fmt ");
    auto data = RiffParser::getSubChunk<WaveDataChunk>(parser, "data");
    auto smpl = RiffParser::getSubChunk<WaveSmplChunk>(parser, "smpl");

    const int32_t bytesPerSample = IntHelper::divideByRoundUp(fmt->getBitsPerSample(), 8);
    structure->riffSubChunks = parser->getAllSubChunks();
    structure->sampleCount = data->getSubChunkSize() / bytesPerSample / fmt->getChannelCount();
    structure->sampleRate = fmt->getSampleRate();
    structure->bitsPerSample = fmt->getBitsPerSample();
    structure->channelCount = fmt->getChannelCount();

    if (smpl != nullptr) {
        auto loops = smpl->getLoops();

        if (loops != nullptr && !loops->empty()) {
            const auto &loop = (*loops)[0];
            structure->loopStart = loop.start;
            structure->loopEnd = loop.end;
            structure->looping = loop.end > loop.start;
        }
    }

    if (readAudioData) {
        switch (fmt->getBitsPerSample()) {
            case 16:
                structure->audioData16 = Interleave::interleavedByteToShort(data->getData(), fmt->getChannelCount());
                break;
            case 8:
                structure->audioData8 = Interleave::deinterleave(data->getData(), bytesPerSample, fmt->getChannelCount());
                break;
            default:
                // Ignore errors
                break;
        }
    }

    return structure;
}

shared_ptr<IAudioFormat> WaveReader::toAudioStream(const shared_ptr<WaveStructure> &structure) {
    switch (structure->bitsPerSample) {
        case 16: {
            auto builder = make_shared<Pcm16FormatBuilder>(structure->audioData16, structure->sampleRate);
            builder->withLoop(structure->looping, structure->loopStart, structure->loopEnd);
            return builder->build();
        }
        case 8: {
            auto builder = make_shared<Pcm8FormatBuilder>(structure->audioData8, structure->sampleRate);
            builder->withLoop(structure->looping, structure->loopStart, structure->loopEnd);
            return builder->build();
        }
        default:
            return nullptr;
    }
}

void WaveReader::validateWaveFile(const shared_ptr<RiffParser> &parser) noexcept(false) {
    if (parser->getRiffChunk()->getType() != "WAVE") {
        throw std::runtime_error("invalid wave file");
    }

    auto fmt = RiffParser::getSubChunk<WaveFmtChunk>(parser, "fmt ");

    if (fmt == nullptr) {
        throw std::runtime_error("file must contain a valid fmt chunk");
    }

    if (RiffParser::getSubChunk<WaveDataChunk>(parser, "data") == nullptr) {
        throw std::runtime_error("file must contain a valid data chunk");
    }

    const auto bytesPerSample = IntHelper::divideByRoundUp(fmt->getBitsPerSample(), 8);

    switch (fmt->getFormatTag()) {
        case WaveFormatTags::Pcm:
        case WaveFormatTags::Extensible:
            break;
        default:
            throw std::runtime_error(StrHelper::format("expected PCM data but contains unsupported format %" PRId32, fmt->getFormatTag()));
    }

    switch (fmt->getBitsPerSample()) {
        case 16:
        case 8:
            break;
        default:
            throw std::runtime_error(StrHelper::format("unsupported bits per sample: %" PRId32, fmt->getBitsPerSample()));
    }

    if (fmt->getChannelCount() <= 0) {
        throw std::runtime_error("invalid channel count");
    }

    if (fmt->getBlockAlign() != bytesPerSample * fmt->getChannelCount()) {
        throw std::runtime_error("invalid block alignment");
    }

    if (fmt->getExtensible() != nullptr) {
        auto format = fmt->getExtensible()->getSubFormat();

        if (format != MediaSubtypes::Pcm) {
            const auto formatDescription = format.toString();
            throw std::runtime_error(StrHelper::format("unsupported subformat %s", formatDescription.c_str()));
        }
    }
}
