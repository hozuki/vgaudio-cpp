#include "../../../common/utilities/macros.h"
#include "SignedPcm8Format.h"
#include "UnsignedPcm8Format.h"
#include "Pcm8FormatBuilder.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm8;

Pcm8FormatBuilder::Pcm8FormatBuilder(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate, bool signed_)
    : _signed(signed_) {
    if (channels == nullptr || channels->size() == 0) {
        throw std::invalid_argument("channels cannot be empty or null");
    }

    setChannels(channels);

    for (const auto &channel : *channels) {
        if (channel == nullptr) {
            throw std::invalid_argument("channel cannot be null");
        }
    }

    setSampleCount((*channels)[0]->size());

    setSampleRate(sampleRate);
    setSigned(signed_);

    for (const auto &channel : *channels) {
        if (channel->size() != getSampleCount()) {
            throw std::invalid_argument("all channels must have the same sample count");
        }
    }
}

shared_ptr<IAudioFormat> Pcm8FormatBuilder::build() {
    auto thiz = shared_ptr<MyClass>(this, DONT_DELETE(MyClass));

    if (isSigned()) {
        return make_shared<SignedPcm8Format>(thiz);
    } else {
        return make_shared<UnsignedPcm8Format>(thiz);
    }
}

int32_t Pcm8FormatBuilder::getChannelCount() const {
    return getChannels()->size();
}

jarray2_ptr<uint8_t> Pcm8FormatBuilder::getChannels() const {
    return _channels;
}

void Pcm8FormatBuilder::setChannels(const jarray2_ptr<uint8_t> &channels) {
    _channels = channels;
}

bool Pcm8FormatBuilder::isSigned() const {
    return _signed;
}

void Pcm8FormatBuilder::setSigned(bool signed_) {
    _signed = signed_;
}
