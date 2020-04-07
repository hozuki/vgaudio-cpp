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

    struct CriHcaPacking {

        IMPLEMENT_STATIC_CLASS(CriHcaPacking);

        static bool unpackFrame(const std::shared_ptr<CriHcaFrame> &frame, const std::shared_ptr<vgaudio::utilities::BitReader> &reader);

        static void packFrame(const std::shared_ptr<CriHcaFrame> &frame, const std::shared_ptr<vgaudio::utilities::Crc16> &crc, const common_lib::utilities::array_ptr<uint8_t> &outBuffer);

        static int32_t calculateResolution(int32_t scaleFactor, int32_t noiseLevel);

    };

}
