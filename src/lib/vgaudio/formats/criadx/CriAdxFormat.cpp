#include <vector>
#include <cinttypes>

#include "CriAdxFormat.h"
#include "CriAdxFormatBuilder.h"
#include "CriAdxChannel.h"
#include "CriAdxHelpers.h"
#include "../../codecs/criadx/CriAdxCodec.h"
#include "../../codecs/criadx/CriAdxParameters.h"
#include "../pcm16/Pcm16Format.h"
#include "../pcm16/Pcm16FormatBuilder.h"
#include "../../IProgressReport.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "../../../common/utilities/StrHelper.h"
#include "../../../common/utilities/IntHelper.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::formats;
using namespace vgaudio::formats::criadx;
using namespace vgaudio::formats::pcm16;
using namespace vgaudio::codecs;
using namespace vgaudio::codecs::criadx;

CriAdxFormat::CriAdxFormat()
    : _highPassFrequency(0), _frameSize(0), _alignmentSamples(0), _version(0), _type(CriAdxType::Linear) {
    _channels = make_array_dynamic<shared_ptr<CriAdxChannel>>(0);
}

CriAdxFormat::CriAdxFormat(const shared_ptr<CriAdxFormatBuilder> &builder)
    : MyBase(builder) {
    _channels = builder->getChannels();
    _frameSize = builder->getFrameSize();
    _highPassFrequency = builder->getHighPassFrequency();
    _type = builder->getType();
    _version = builder->getVersion();
    _alignmentSamples = builder->getAlignmentSamples();
}

array_ptr<shared_ptr<CriAdxChannel>> CriAdxFormat::getChannels() const {
    return _channels;
}

int16_t CriAdxFormat::getHighPassFrequency() const {
    return _highPassFrequency;
}

int32_t CriAdxFormat::getFrameSize() const {
    return _frameSize;
}

int32_t CriAdxFormat::getAlignmentSamples() const {
    return _alignmentSamples;
}

int32_t CriAdxFormat::getVersion() const {
    return _version;
}

CriAdxType CriAdxFormat::getType() const {
    return _type;
}

int32_t CriAdxFormat::getSampleCount() const {
    return getUnalignedSampleCount() + getAlignmentSamples();
}

int32_t CriAdxFormat::getLoopStart() const {
    return getUnalignedLoopStart() + getAlignmentSamples();
}

int32_t CriAdxFormat::getLoopEnd() const {
    return getUnalignedLoopEnd() + getAlignmentSamples();
}

shared_ptr<Pcm16Format> CriAdxFormat::toPcm16() {
    const auto cfg = make_shared<CriAdxParameters>();
    return toPcm16(cfg);
}

shared_ptr<Pcm16Format> CriAdxFormat::toPcm16(const shared_ptr<CodecParameters> &config) {
    const auto channels = getChannels();
    const auto channelCount = channels->size();

    const auto pcmChannels = make_jagged_array_2_dynamic<int16_t>(channelCount);

    const int32_t samplesPerFrame = (getFrameSize() - 2) * 2;
    const auto sampleCount = getUnalignedSampleCount();
    const auto framesPerChannel = IntHelper::divideByRoundUp(sampleCount, samplesPerFrame);

    const auto progress = config->progress;

    if (progress != nullptr) {
        progress->setTotal(framesPerChannel * channelCount);
    }

    // TODO: Can be parallel
    for (auto i = 0; i < channelCount; i += 1) {
        const auto channelConfig = make_shared<CriAdxParameters>();
        channelConfig->progress = progress;
        channelConfig->sampleRate = getSampleRate();
        channelConfig->frameSize = getFrameSize();
        channelConfig->padding = getAlignmentSamples();
        channelConfig->highPassFrequency = getHighPassFrequency();
        channelConfig->type = getType();
        channelConfig->version = getVersion();

        const auto channel = CriAdxCodec::decode((*channels)[i]->audio, sampleCount, channelConfig);
        (*pcmChannels)[i] = channel;
    }

    const auto builder = make_shared<Pcm16FormatBuilder>(pcmChannels, getSampleRate());
    builder->withLoop(isLooping(), getUnalignedLoopStart(), getUnalignedLoopEnd());
    builder->withTracks(getTracks());

    const auto pcm16 = builder->build();
    return dynamic_pointer_cast<Pcm16Format>(pcm16);
}

shared_ptr<IAudioFormat> CriAdxFormat::encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) {
    const auto config = make_shared<CriAdxParameters>();
    return encodeFromPcm16WithConfig(pcm16, config);
}

shared_ptr<IAudioFormat> CriAdxFormat::encodeFromPcm16WithConfig(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) {
    const auto cfg = dynamic_pointer_cast<CriAdxParameters>(config);
    const auto progress = config->progress;

    const auto channelCount = pcm16->getChannelCount();

    const auto channels = make_array_dynamic<shared_ptr<CriAdxChannel>>(channelCount);
    const int32_t samplesPerFrame = (cfg->frameSize - 2) * 2;
    const int32_t alignmentMultiple = channelCount == 1 ? samplesPerFrame * 2 : samplesPerFrame;
    const int32_t alignmentSamples = IntHelper::getNextMultiple(pcm16->getLoopStart(), alignmentMultiple) - pcm16->getLoopStart();

    const auto byteCount = CriAdxHelpers::sampleCountToByteCount(pcm16->getSampleCount(), cfg->frameSize);
    const auto framesPerChannel = IntHelper::divideByRoundUp(byteCount, cfg->frameSize);

    if (progress != nullptr) {
        progress->setTotal(framesPerChannel * channelCount);
    }

    const auto sourceChannels = pcm16->getChannels();

    // TODO: Can be parallel
    for (auto i = 0; i < channelCount; i += 1) {
        const auto channelConfig = make_shared<CriAdxParameters>();
        channelConfig->progress = progress;
        channelConfig->sampleRate = pcm16->getSampleRate();
        channelConfig->frameSize = cfg->frameSize;
        channelConfig->padding = alignmentSamples;
        channelConfig->filter = cfg->filter;
        channelConfig->type = cfg->type;
        channelConfig->version = cfg->version;

        const auto adpcm = CriAdxCodec::encode((*sourceChannels)[i], channelConfig);
        const auto channel = make_shared<CriAdxChannel>(adpcm, channelConfig->history, channelConfig->version);
        (*channels)[i] = channel;
    }

    auto builder = make_shared<CriAdxFormatBuilder>(channels, pcm16->getSampleCount(), pcm16->getSampleRate(), cfg->frameSize, 500);
    builder->withLoop(pcm16->isLooping(), pcm16->getLoopStart(), pcm16->getLoopEnd());
    builder->setAlignmentSamples(alignmentSamples);
    builder->setType(cfg->type);

    return builder->build();
}

shared_ptr<AudioFormatBase> CriAdxFormat::getChannelsInternal(const array_ptr<int32_t> &channelRange) {
    const auto channelArray = getChannels();

    vector<shared_ptr<CriAdxChannel>> channels;

    for (const auto i : *channelRange) {
        if (i < 0 || i >= channelArray->size()) {
            throw std::invalid_argument(StrHelper::format("Channel %" PRId32 " does not exist.", i));
        }

        channels.push_back((*channelArray)[i]);
    }

    auto builderCopy = getCloneBuilder();
    auto copy = dynamic_pointer_cast<CriAdxFormatBuilder>(builderCopy);

    const auto newChannels = ArrayHelper::toArray(channels);
    copy->setChannels(newChannels);

    auto result = copy->build();

    return dynamic_pointer_cast<AudioFormatBase>(result);
}

shared_ptr<AudioFormatBase> CriAdxFormat::addInternal(const shared_ptr<IAudioFormat> &format) {
    auto builderCopy = getCloneBuilder();
    auto copy = dynamic_pointer_cast<CriAdxFormatBuilder>(builderCopy);
    auto f = dynamic_pointer_cast<CriAdxFormat>(format);

    const auto newChannels = ArrayHelper::concat(getChannels(), f->getChannels());
    copy->setChannels(newChannels);

    auto result = copy->build();

    return dynamic_pointer_cast<AudioFormatBase>(result);
}

std::shared_ptr<AudioFormatBaseBuilder> CriAdxFormat::getCloneBuilder() {
    auto builder = make_shared<CriAdxFormatBuilder>(getChannels(), getSampleCount(), getSampleRate(), getFrameSize(), getHighPassFrequency());
    getCloneBuilderBase(builder);
    builder->setAlignmentSamples(getAlignmentSamples());
    return builder;
}

bool CriAdxFormat::canAcceptAudioFormat(const IAudioFormat *format) const {
    return dynamic_cast<const CriAdxFormat *>(format) != nullptr;
}

bool CriAdxFormat::canAcceptConfig(const CodecParameters *config) const {
    return dynamic_cast<const CriAdxParameters *>(config) != nullptr;
}

shared_ptr<CodecParameters> CriAdxFormat::createConfig() const {
    return make_shared<CriAdxParameters>();
}
