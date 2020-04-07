#include "../../codecs/pcm8/Pcm8Codec.h"
#include "../../codecs/CodecParameters.h"
#include "UnsignedPcm8Format.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs;
using namespace vgaudio::codecs::pcm8;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm8;

UnsignedPcm8Format::UnsignedPcm8Format(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate)
    : MyBase(channels, sampleRate) {
}

UnsignedPcm8Format::UnsignedPcm8Format(const shared_ptr<Pcm8FormatBuilder> &builder)
    : MyBase(builder) {
}

bool UnsignedPcm8Format::isSigned() const {
    return false;
}

function<array_ptr<uint8_t>(const array_ptr<int16_t> &)> UnsignedPcm8Format::getEncodeFunction() const {
    return Pcm8Codec::encodeUnsigned;
}

function<array_ptr<int16_t>(const array_ptr<uint8_t> &)> UnsignedPcm8Format::getDecodeFunction() const {
    return Pcm8Codec::decodeUnsigned;
}

bool UnsignedPcm8Format::canAcceptAudioFormat(const IAudioFormat *format) const {
    return dynamic_cast<const UnsignedPcm8Format *>(format) != nullptr;
}

bool UnsignedPcm8Format::canAcceptConfig(const CodecParameters *config) const {
    return dynamic_cast<const CodecParameters *>(config) != nullptr;
}
