#pragma once

#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio::codecs::criadx {

    struct CriAdxParameters;

    struct CriAdxCodec {

        IMPLEMENT_STATIC_CLASS(CriAdxCodec);

        static common_lib::utilities::array_ptr<int16_t> decode(const common_lib::utilities::array_ptr<uint8_t> &adpcm, int32_t sampleCount, const std::shared_ptr<CriAdxParameters> &config = nullptr);

        static common_lib::utilities::array_ptr<uint8_t> encode(const common_lib::utilities::array_ptr<int16_t> &pcm, const std::shared_ptr<CriAdxParameters> &config);

    };

}
