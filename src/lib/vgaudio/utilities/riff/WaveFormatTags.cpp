#include "WaveFormatTags.h"
#include "../../../common/utilities/_cxx17.h"

#if !_CXX17_AVAILABLE
namespace vgaudio {
    namespace utilities {
        namespace riff {

            // https://stackoverflow.com/a/28846608
            constexpr int32_t WaveFormatTags::Pcm;
            constexpr int32_t WaveFormatTags::Extensible;

        }
    }
}
#endif
