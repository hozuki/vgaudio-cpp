#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct HcaInfo;
            struct CriHcaParameters;

            struct CriHcaDecoder {

                IMPLEMENT_STATIC_CLASS(CriHcaDecoder);

                static common_lib::utilities::jarray2_ptr<int16_t> decode(const std::shared_ptr<HcaInfo> &hca, const common_lib::utilities::jarray2_ptr<uint8_t> &audio, const std::shared_ptr<CriHcaParameters> &config = nullptr);

                static void copyBuffer(const common_lib::utilities::jarray2_ptr<int16_t> &bufferIn, const common_lib::utilities::jarray2_ptr<int16_t> &bufferOut, int32_t startIndex, int32_t bufferIndex);

            };

        }
    }
}
