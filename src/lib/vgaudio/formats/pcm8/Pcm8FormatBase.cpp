#include <vector>
#include <cinttypes>

#include "../../../common/utilities/ArrayHelper.h"
#include "../pcm16/Pcm16FormatBuilder.h"
#include "../pcm16/Pcm16Format.h"
#include "Pcm8FormatBuilder.h"
#include "Pcm8FormatBase.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm8;
using namespace vgaudio::formats::pcm16;

Pcm8FormatBase::Pcm8FormatBase() {
    _channels = make_jagged_array_2_dynamic<uint8_t>(0);
}

Pcm8FormatBase::Pcm8FormatBase(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate)
    : MyClass(make_shared<Pcm8FormatBuilder>(channels, sampleRate)) {
}

Pcm8FormatBase::Pcm8FormatBase(const shared_ptr<Pcm8FormatBuilder> &builder)
    : MyBase(builder) {
    _channels = builder->getChannels();
}

shared_ptr<Pcm16Format> Pcm8FormatBase::toPcm16() {
    const auto channelCount = getChannelCount();
    auto channels = make_jagged_array_2_dynamic<int16_t>(channelCount);
    const auto &sourceChannels = getChannels();

    const auto decodeFunc = getDecodeFunction();
    for (auto i = 0; i < channelCount; i += 1) {
        (*channels)[i] = decodeFunc((*sourceChannels)[i]);
    }

    auto builder = make_shared<Pcm16FormatBuilder>(channels, getSampleRate());
    builder->withLoop(isLooping(), getLoopStart(), getLoopEnd());
    builder->withTracks(getTracks());
    return dynamic_pointer_cast<Pcm16Format>(builder->build());
}

shared_ptr<IAudioFormat> Pcm8FormatBase::encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) {
    const auto channelCount = pcm16->getChannelCount();
    auto channels = make_jagged_array_2_dynamic<uint8_t>(channelCount);
    const auto &sourceChannels = pcm16->getChannels();

    const auto encodeFunc = getEncodeFunction();
    for (auto i = 0; i < channelCount; i += 1) {
        (*channels)[i] = encodeFunc((*sourceChannels)[i]);
    }

    auto builder = make_shared<Pcm8FormatBuilder>(channels, pcm16->getSampleRate(), isSigned());
    builder->withLoop(pcm16->isLooping(), pcm16->getLoopStart(), pcm16->getLoopEnd());
    builder->withTracks(pcm16->getTracks());
    return builder->build();
}

jarray2_ptr<uint8_t> Pcm8FormatBase::getChannels() const {
    return _channels;
}

shared_ptr<AudioFormatBase> Pcm8FormatBase::addInternal(const shared_ptr<IAudioFormat> &format) {
    shared_ptr<Pcm8FormatBase> pcm8 = dynamic_pointer_cast<Pcm8FormatBase>(format);
    shared_ptr<Pcm8FormatBuilder> copy = dynamic_pointer_cast<Pcm8FormatBuilder>(getCloneBuilder());
    copy->setChannels(ArrayHelper::concat(getChannels(), pcm8->getChannels()));
    return dynamic_pointer_cast<AudioFormatBase>(copy->build());
}

shared_ptr<AudioFormatBase> Pcm8FormatBase::getChannelsInternal(const array_ptr<int32_t> &channelRange) {
    vector<array_ptr<uint8_t>> channels;
    const auto &myChannels = getChannels();

    for (auto i : *channelRange) {
        if (i < 0 || i >= myChannels->size()) {
            throw std::runtime_error(StrHelper::format("channel %" PRId32 " does not exist", i));
        }

        channels.push_back((*myChannels)[i]);
    }

    shared_ptr<Pcm8FormatBuilder> copy = dynamic_pointer_cast<Pcm8FormatBuilder>(getCloneBuilder());
    copy->setChannels(ArrayHelper::toArray(channels));
    return dynamic_pointer_cast<AudioFormatBase>(copy->build());
}

shared_ptr<Pcm8FormatBuilder> Pcm8FormatBase::getBuilder(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate) {
    return make_shared<Pcm8FormatBuilder>(channels, sampleRate);
}

shared_ptr<AudioFormatBaseBuilder> Pcm8FormatBase::getCloneBuilder() {
    auto builder = getBuilder(getChannels(), getSampleRate());
    return getCloneBuilderBase(builder);
}
