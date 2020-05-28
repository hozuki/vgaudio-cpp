#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaConstants {

                IMPLEMENT_STATIC_CLASS(CriHcaConstants);

                constexpr static int32_t SubframesPerFrame = 8;
                constexpr static int32_t SubframeSampleBits = 7;
                constexpr static int32_t SamplesPerSubframe = 1 << SubframeSampleBits;
                constexpr static int32_t SamplesPerFrame = SubframesPerFrame * SamplesPerSubframe;

            };

        }
    }
}
