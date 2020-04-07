#pragma once

#include <cstdint>

#include "../../../common/utilities/static_class.h"

namespace vgaudio::formats::criadx {

    struct CriAdxHelpers {

        IMPLEMENT_STATIC_CLASS(CriAdxHelpers);

        static int32_t nibbleCountToSampleCount(int32_t nibbleCount, int32_t frameSize);

        static int32_t sampleCountToNibbleCount(int32_t sampleCount, int32_t frameSize);

        static int32_t sampleCountToByteCount(int32_t sampleCount, int32_t frameSize);

    };

}
