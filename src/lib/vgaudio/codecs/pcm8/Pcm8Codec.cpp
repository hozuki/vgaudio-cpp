#include "Pcm8Codec.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::pcm8;

array_ptr<uint8_t> Pcm8Codec::encodeUnsigned(const array_ptr<int16_t> &array) {
    const auto length = array->size();
    auto output = make_array_dynamic<uint8_t>(length);

    for (auto i = 0; i < length; i += 1) {
        (*output)[i] = static_cast<uint8_t>(((*array)[i] + INT16_MAX + 1) >> 8);
    }

    return output;
}

array_ptr<int16_t> Pcm8Codec::decodeUnsigned(const array_ptr<uint8_t> &array) {
    const auto length = array->size();
    auto output = make_array_dynamic<int16_t>(length);

    for (auto i = 0; i < length; i += 1) {
        (*output)[i] = static_cast<int16_t>(((*array)[i] - 0x80) << 8);
    }

    return output;
}

array_ptr<uint8_t> Pcm8Codec::encodeSigned(const array_ptr<int16_t> &array) {
    const auto length = array->size();
    auto output = make_array_dynamic<uint8_t>(length);

    for (auto i = 0; i < length; i += 1) {
        (*output)[i] = static_cast<uint8_t>((*array)[i] >> 8);
    }

    return output;
}

array_ptr<int16_t> Pcm8Codec::decodeSigned(const array_ptr<uint8_t> &array) {
    const auto length = array->size();
    auto output = make_array_dynamic<int16_t>(length);

    for (auto i = 0; i < length; i += 1) {
        (*output)[i] = static_cast<int16_t>((*array)[i] << 8);
    }

    return output;
}
