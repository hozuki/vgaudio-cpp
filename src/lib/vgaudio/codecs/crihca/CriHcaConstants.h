#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaConstants {

                IMPLEMENT_STATIC_CLASS(CriHcaConstants);

                static constexpr int32_t SubframesPerFrame = 8;
                static constexpr int32_t SubframeSampleBits = 7;
                static constexpr int32_t SamplesPerSubframe = 1 << SubframeSampleBits;
                static constexpr int32_t SamplesPerFrame = SubframesPerFrame * SamplesPerSubframe;

            };

        }
    }
}
