#include "../../codecs/pcm8/Pcm8Codec.h"
#include "../../codecs/CodecParameters.h"
#include "SignedPcm8Format.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs;
using namespace vgaudio::codecs::pcm8;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm8;

SignedPcm8Format::SignedPcm8Format(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate)
    : MyBase(channels, sampleRate) {
}

SignedPcm8Format::SignedPcm8Format(const shared_ptr<Pcm8FormatBuilder> &builder)
    : MyBase(builder) {
}

bool SignedPcm8Format::isSigned() const {
    return true;
}

function<array_ptr<uint8_t>(const array_ptr<int16_t> &)> SignedPcm8Format::getEncodeFunction() const {
    return Pcm8Codec::encodeSigned;
}

function<array_ptr<int16_t>(const array_ptr<uint8_t> &)> SignedPcm8Format::getDecodeFunction() const {
    return Pcm8Codec::decodeSigned;
}

bool SignedPcm8Format::canAcceptAudioFormat(const IAudioFormat *format) const {
    return dynamic_cast<const SignedPcm8Format *>(format) != nullptr;
}

bool SignedPcm8Format::canAcceptConfig(const CodecParameters *config) const {
    return dynamic_cast<const CodecParameters *>(config) != nullptr;
}
