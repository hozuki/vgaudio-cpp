#pragma once

#include <cstdint>
#include <memory>

#include "../../../../common/utilities/runtime_array.h"
#include "../../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {
            struct CriHcaFrame;
            struct HcaInfo;
        }
    }
}

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

void DecodeFrame(const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaFrame> &frame, const jarray2_ptr<int16_t> &pcmOut);

void CopyPcmToOutput(const jarray2_ptr<int16_t> &pcmIn, const jarray2_ptr<int16_t> &pcmOut, const shared_ptr<HcaInfo> &hca, int32_t frame);
