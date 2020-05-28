#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace codecs {
        namespace pcm8 {

            struct Pcm8Codec {

                IMPLEMENT_STATIC_CLASS(Pcm8Codec);

            public:

                static common_lib::utilities::array_ptr<uint8_t> encodeUnsigned(const common_lib::utilities::array_ptr<int16_t> &array);

                static common_lib::utilities::array_ptr<int16_t> decodeUnsigned(const common_lib::utilities::array_ptr<uint8_t> &array);

                static common_lib::utilities::array_ptr<uint8_t> encodeSigned(const common_lib::utilities::array_ptr<int16_t> &array);

                static common_lib::utilities::array_ptr<int16_t> decodeSigned(const common_lib::utilities::array_ptr<uint8_t> &array);

            };

        }
    }
}
