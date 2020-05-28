#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct WaveFormatTags {

                IMPLEMENT_STATIC_CLASS(WaveFormatTags);

            public:

                static constexpr int32_t Pcm = 0x0001;

                static constexpr int32_t Extensible = 0xfffe;

            };

        }
    }
}
