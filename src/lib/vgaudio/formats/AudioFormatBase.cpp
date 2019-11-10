#include "pcm16/Pcm16Format.h"
#include "AudioFormatBaseBuilder.h"
#include "../codecs/CodecParameters.h"
#include "AudioTrack.h"
#include "AudioFormatBase.h"

using namespace std;
using namespace vgaudio::formats;

AudioFormatBase::AudioFormatBase()
    : _sampleRate(0), _channelCount(0), _unalignedSampleCount(0),
      _unalignedLoopStart(0), _unalignedLoopEnd(0), _looping(false) {
}

AudioFormatBase::AudioFormatBase(const shared_ptr<AudioFormatBaseBuilder> &builder) {
    _unalignedSampleCount = builder->getSampleCount();
    _sampleRate = builder->getSampleRate();
    _channelCount = builder->getChannelCount();
    _looping = builder->isLooping();
    _unalignedLoopStart = builder->getLoopStart();
    _unalignedLoopEnd = builder->getLoopEnd();

    _tracks = builder->getTracks();

    if (_tracks == nullptr || _tracks->empty()) {
        _tracks = AudioTrack::getDefaultTrackList(getChannelCount());
    }
}

int32_t AudioFormatBase::getSampleRate() const {
    return _sampleRate;
}

int32_t AudioFormatBase::getChannelCount() const {
    return _channelCount;
}

int32_t AudioFormatBase::getUnalignedSampleCount() const {
    return _unalignedSampleCount;
}

int32_t AudioFormatBase::getUnalignedLoopStart() const {
    return _unalignedLoopStart;
}

int32_t AudioFormatBase::getUnalignedLoopEnd() const {
    return _unalignedLoopEnd;
}

int32_t AudioFormatBase::getSampleCount() const {
    return getUnalignedSampleCount();
}

int32_t AudioFormatBase::getLoopStart() const {
    return getUnalignedLoopStart();
}

int32_t AudioFormatBase::getLoopEnd() const {
    return getUnalignedLoopEnd();
}

bool AudioFormatBase::isLooping() const {
    return _looping;
}

shared_ptr<list<shared_ptr<AudioTrack>>> AudioFormatBase::getTracks() const {
    return _tracks;
}

shared_ptr<IAudioFormat> AudioFormatBase::getChannels(const array_ptr<int32_t> &channelRange) {
    return getChannelsInternal(channelRange);
}

shared_ptr<IAudioFormat> AudioFormatBase::withLoop(bool loop) {
    auto builder = getCloneBuilder();
    builder->withLoop(loop);
    return builder->build();
}

shared_ptr<IAudioFormat> AudioFormatBase::withLoop(bool loop, int32_t loopStart, int32_t loopEnd) {
    auto builder = getCloneBuilder();
    builder->withLoop(loop, loopStart, loopEnd);
    return builder->build();
}

bool AudioFormatBase::tryAdd(const shared_ptr<IAudioFormat> &format, shared_ptr<IAudioFormat> &result) {
    result = nullptr;

    if (!canAcceptAudioFormat(format.get())) {
        return false;
    }

    try {
        addInternal(format);
    } catch (...) {
        return false;
    }

    return true;
}

shared_ptr<AudioFormatBase> AudioFormatBase::add(const shared_ptr<AudioFormatBase> &format) {
    if (!canAcceptAudioFormat(format.get())) {
        throw std::invalid_argument("type invalid: format");
    }

    if (format->getUnalignedSampleCount() != this->getUnalignedSampleCount()) {
        throw std::invalid_argument("streams must have the same length");
    }

    return addInternal(format);
}

shared_ptr<AudioFormatBaseBuilder> AudioFormatBase::getCloneBuilderBase(const shared_ptr<AudioFormatBaseBuilder> &builder) {
    builder->setSampleCount(getUnalignedSampleCount());
    builder->setSampleRate(getSampleRate());
    builder->setLooping(isLooping());
    builder->setLoopStart(getUnalignedLoopStart());
    builder->setLoopEnd(getUnalignedLoopEnd());
    builder->setTracks(getTracks());

    return builder;
}

shared_ptr<CodecParameters> AudioFormatBase::getDerivedParameters(const shared_ptr<CodecParameters> &param) {
    if (param == nullptr) {
        return nullptr;
    }

    if (canAcceptConfig(param.get())) {
        return param;
    }

    auto config = createConfig();

    config->sampleCount = param->sampleCount;
    config->progress = param->progress;

    return config;
}

shared_ptr<IAudioFormat> AudioFormatBase::encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) {
    return encodeFromPcm16WithConfig(pcm16, getDerivedParameters(config));
}

shared_ptr<IAudioFormat> AudioFormatBase::encodeFromPcm16WithConfig(const shared_ptr<Pcm16Format> &pcm16, [[maybe_unused]] const shared_ptr<CodecParameters> &config) {
    return encodeFromPcm16(pcm16);
}

shared_ptr<Pcm16Format> AudioFormatBase::toPcm16(const shared_ptr<CodecParameters> &config) {
    return toPcm16();
}

shared_ptr<CodecParameters> AudioFormatBase::createConfig() const {
    return make_shared<CodecParameters>();
}
