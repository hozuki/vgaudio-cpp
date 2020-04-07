#include <stdexcept>

#include "CriAdxFormatBuilder.h"
#include "CriAdxFormat.h"
#include "CriAdxChannel.h"
#include "../../../common/utilities/macros.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::formats;
using namespace vgaudio::formats::criadx;
using namespace vgaudio::codecs::criadx;

CriAdxFormatBuilder::CriAdxFormatBuilder(const array_ptr<shared_ptr<CriAdxChannel>> &channels, int32_t sampleCount, int32_t sampleRate, int32_t frameSize, int16_t highPassFrequency)
    : _highPassFrequency(0), _frameSize(0), _alignmentSamples(0), _type(CriAdxType::Linear) {
    setChannels(channels);
    setSampleCount(sampleCount);
    setSampleRate(sampleRate);
    setFrameSize(frameSize);
    setHighPassFrequency(highPassFrequency);
}

int32_t CriAdxFormatBuilder::getChannelCount() const {
    return _channels->size();
}

shared_ptr<IAudioFormat> CriAdxFormatBuilder::build() {
    auto thiz = shared_ptr<MyClass>(this, DONT_DELETE(MyClass));
    return make_shared<CriAdxFormat>(thiz);
}

array_ptr<shared_ptr<CriAdxChannel>> CriAdxFormatBuilder::getChannels() const {
    return _channels;
}

void CriAdxFormatBuilder::setChannels(const array_ptr<shared_ptr<CriAdxChannel>> &channels) {
    if (channels == nullptr || channels->empty()) {
        throw std::invalid_argument("Channels cannot be empty or null.");
    }

    for (const auto &channel : *channels) {
        if (channel == nullptr) {
            throw std::invalid_argument("All channels must be non-null.");
        }

        if (channel->audio == nullptr) {
            throw std::invalid_argument("Audio in channel cannot be null");
        }
    }

    const auto length = (*channels)[0]->audio->size();

    for (const auto &channel : *channels) {
        if (channel->audio->size() != length) {
            throw std::invalid_argument("All channels must have the same length.");
        }
    }

    _channels = channels;
}

int16_t CriAdxFormatBuilder::getHighPassFrequency() const {
    return _highPassFrequency;
}

void CriAdxFormatBuilder::setHighPassFrequency(int16_t frequency) {
    _highPassFrequency = frequency;
}

int32_t CriAdxFormatBuilder::getFrameSize() const {
    return _frameSize;
}

void CriAdxFormatBuilder::setFrameSize(int32_t frameSize) {
    _frameSize = frameSize;
}

int32_t CriAdxFormatBuilder::getAlignmentSamples() const {
    return _alignmentSamples;
}

void CriAdxFormatBuilder::setAlignmentSamples(int32_t samples) {
    _alignmentSamples = samples;
}

CriAdxType CriAdxFormatBuilder::getType() const {
    return _type;
}

void CriAdxFormatBuilder::setType(CriAdxType type) {
    _type = type;
}

int32_t CriAdxFormatBuilder::getVersion() const {
    return (*_channels)[0]->version;
}
