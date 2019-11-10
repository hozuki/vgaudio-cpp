#include "../../../common/utilities/macros.h"
#include "Pcm16Format.h"
#include "Pcm16FormatBuilder.h"

using namespace std;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm16;
using namespace common_lib::utilities;

Pcm16FormatBuilder::Pcm16FormatBuilder(const jarray2_ptr<int16_t> &channels, int32_t sampleRate) {
    if (channels == nullptr || channels->empty()) {
        throw std::invalid_argument("invalid channels");
    }

    setChannels(channels);

    for (const auto &channel : *channels) {
        if (channel == nullptr) {
            throw std::invalid_argument("channel must be non-null");
        }
    }

    setSampleCount((*channels)[0]->size());
    setSampleRate(sampleRate);

    for (const auto &channel : *getChannels()) {
        if (channel->size() != getSampleCount()) {
            throw std::invalid_argument("all channels must have the same sample count");
        }
    }
}

jarray2_ptr<int16_t> Pcm16FormatBuilder::getChannels() const {
    return _channels;
}

void Pcm16FormatBuilder::setChannels(const jarray2_ptr<int16_t> &channels) {
    _channels = channels;
}

int32_t Pcm16FormatBuilder::getChannelCount() const {
    return getChannels()->size();
}

shared_ptr<IAudioFormat> Pcm16FormatBuilder::build() {
    auto thiz = shared_ptr<MyClass>(this, DONT_DELETE(MyClass));
    return make_shared<Pcm16Format>(thiz);
}
