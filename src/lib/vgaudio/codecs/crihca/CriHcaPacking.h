#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio::utilities {
    struct Crc16;
    struct BitReader;
}

namespace vgaudio::codecs::crihca {

    struct CriHcaFrame;

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::utilities;

    struct CriHcaPacking {

        IMPLEMENT_STATIC_CLASS(CriHcaPacking);

        static bool unpackFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader);

        static void packFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<Crc16> &crc, const array_ptr<uint8_t> &outBuffer);

        static int32_t calculateResolution(int32_t scaleFactor, int32_t noiseLevel);

    };

}
