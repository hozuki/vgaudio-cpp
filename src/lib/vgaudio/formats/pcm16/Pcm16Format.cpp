#include <vector>
#include <cinttypes>

#include "../../../common/utilities/StrHelper.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "Pcm16FormatBuilder.h"
#include "../../codecs/CodecParameters.h"
#include "Pcm16Format.h"

using namespace std;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm16;
using namespace common_lib::utilities;

Pcm16Format::Pcm16Format(const shared_ptr<Pcm16FormatBuilder> &builder)
    : MyBase(builder) {
    _channels = builder->getChannels();
}

Pcm16Format::Pcm16Format(const jarray2_ptr<int16_t> &channels, int32_t sampleRate)
    : MyClass(make_shared<Pcm16FormatBuilder>(channels, sampleRate)) {
}

Pcm16Format::Pcm16Format() {
    _channels = make_jagged_array_2_dynamic<int16_t>(0, 0);
}

jarray2_ptr<int16_t> Pcm16Format::getChannels() const {
    return _channels;
}

shared_ptr<Pcm16Format> Pcm16Format::toPcm16() {
    auto builder = getCloneBuilder();
    auto r = builder->build();
    return dynamic_pointer_cast<Pcm16Format>(r);
}

shared_ptr<IAudioFormat> Pcm16Format::encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) {
    auto builder = pcm16->getCloneBuilder();
    return builder->build();
}

bool Pcm16Format::canAcceptAudioFormat(const IAudioFormat *format) const {
    return dynamic_cast<const Pcm16Format *>(format) != nullptr;
}

bool Pcm16Format::canAcceptConfig(const CodecParameters *config) const {
    return dynamic_cast<const CodecParameters *>(config) != nullptr;
}

shared_ptr<AudioFormatBase> Pcm16Format::addInternal(const shared_ptr<IAudioFormat> &format) {
    auto copy = dynamic_pointer_cast<Pcm16FormatBuilder>(getCloneBuilder());
    auto pcm16 = dynamic_pointer_cast<Pcm16Format>(format);

    auto newChannels = ArrayHelper::concat(getChannels(), pcm16->getChannels());
    copy->setChannels(newChannels);

    return dynamic_pointer_cast<AudioFormatBase>(copy->build());
}

shared_ptr<AudioFormatBase> Pcm16Format::getChannelsInternal(const array_ptr<int32_t> &channelRange) {
    vector<array_ptr<int16_t>> channels;
    const auto &c = getChannels();

    for (auto i : *channelRange) {
        if (i < 0 || i >= c->size()) {
            throw std::invalid_argument(StrHelper::format("channel %" PRId32 " does not exist", i));
        }

        channels.push_back((*c)[i]);
    }

    auto copy = dynamic_pointer_cast<Pcm16FormatBuilder>(getCloneBuilder());
    auto channelArray = ArrayHelper::toArray(channels);
    copy->setChannels(channelArray);
    return dynamic_pointer_cast<AudioFormatBase>(copy->build());
}

shared_ptr<AudioFormatBaseBuilder> Pcm16Format::getCloneBuilder() {
    return getCloneBuilderBase(getBuilder(getChannels(), getSampleRate()));
}

shared_ptr<Pcm16FormatBuilder> Pcm16Format::getBuilder(const jarray2_ptr<int16_t> &channels, int32_t sampleRate) {
    return make_shared<Pcm16FormatBuilder>(channels, sampleRate);
}
