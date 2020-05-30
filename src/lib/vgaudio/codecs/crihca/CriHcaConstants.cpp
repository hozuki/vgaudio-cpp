#include "CriHcaConstants.h"
#include "../../../common/utilities/_cxx17.h"

#if !_CXX17_AVAILABLE
namespace vgaudio {
    namespace codecs {
        namespace crihca {

            // https://stackoverflow.com/a/28846608
            constexpr int32_t CriHcaConstants::SubframesPerFrame;
            constexpr int32_t CriHcaConstants::SubframeSampleBits;
            constexpr int32_t CriHcaConstants::SamplesPerSubframe;
            constexpr int32_t CriHcaConstants::SamplesPerFrame;

        }
    }
}
#endif
