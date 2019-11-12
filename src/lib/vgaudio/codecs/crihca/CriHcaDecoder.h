#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs::crihca {

    struct HcaInfo;
    struct CriHcaParameters;

    using namespace std;
    using namespace common_lib::utilities;

    struct CriHcaDecoder {

        IMPLEMENT_STATIC_CLASS(CriHcaDecoder);

        static jarray2_ptr<int16_t> decode(const shared_ptr<HcaInfo>& hca, const jarray2_ptr<uint8_t>& audio, const shared_ptr<CriHcaParameters>& config = nullptr);

        static void copyBuffer(const jarray2_ptr<int16_t>& bufferIn, const jarray2_ptr<int16_t>& bufferOut, int32_t startIndex, int32_t bufferIndex);

    };

}
