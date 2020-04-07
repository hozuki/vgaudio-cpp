#pragma once

#include <memory>

#include "../../../common/utilities/runtime_jagged_array.h"
#include "../../../common/utilities/static_class.h"

namespace vgaudio::codecs::criadx {

    struct CriAdxKey;

    struct CriAdxEncryption {

        IMPLEMENT_STATIC_CLASS(CriAdxEncryption);

        static void encryptDecrypt(const common_lib::utilities::jarray2_ptr<uint8_t> &adpcm, const std::shared_ptr<CriAdxKey> &key, int32_t encryptionType, int32_t frameSize);

    };

}
