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
            struct CriHcaKey;

            struct CriHcaEncryption {

                IMPLEMENT_STATIC_CLASS(CriHcaEncryption);

            public:

                static void encrypt(const std::shared_ptr<HcaInfo> &hca, const common_lib::utilities::jarray2_ptr<uint8_t> &audio, const std::shared_ptr<CriHcaKey> &key);

                static void decrypt(const std::shared_ptr<HcaInfo> &hca, const common_lib::utilities::jarray2_ptr<uint8_t> &audio, const std::shared_ptr<CriHcaKey> &key);

                static void encryptFrame(const std::shared_ptr<HcaInfo> &hca, const common_lib::utilities::array_ptr<uint8_t> &audio, const std::shared_ptr<CriHcaKey> &key);

                static void decryptFrame(const std::shared_ptr<HcaInfo> &hca, const common_lib::utilities::array_ptr<uint8_t> &audio, const std::shared_ptr<CriHcaKey> &key);

            };

        }
    }
}
