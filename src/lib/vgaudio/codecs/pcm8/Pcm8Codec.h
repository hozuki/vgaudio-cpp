#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio::codecs::pcm8 {

    using namespace common_lib::utilities;

    struct Pcm8Codec {

        IMPLEMENT_STATIC_CLASS(Pcm8Codec);

    public:

        static array_ptr<uint8_t> encodeUnsigned(const array_ptr<int16_t> &array);

        static array_ptr<int16_t> decodeUnsigned(const array_ptr<uint8_t> &array);

        static array_ptr<uint8_t> encodeSigned(const array_ptr<int16_t> &array);

        static array_ptr<int16_t> decodeSigned(const array_ptr<uint8_t> &array);

    };

}
