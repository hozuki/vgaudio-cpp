#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array_2.h"

namespace vgaudio::codecs::crihca {

    struct HcaInfo;
    struct CriHcaKey;

    using namespace std;
    using namespace common_lib::utilities;

    struct CriHcaEncryption {

        IMPLEMENT_STATIC_CLASS(CriHcaEncryption);

    public:

        static void encrypt(const shared_ptr<HcaInfo> &hca, const jarray2_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key);

        static void decrypt(const shared_ptr<HcaInfo> &hca, const jarray2_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key);

        static void encryptFrame(const shared_ptr<HcaInfo> &hca, const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key);

        static void decryptFrame(const shared_ptr<HcaInfo> &hca, const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key);

    };

}
