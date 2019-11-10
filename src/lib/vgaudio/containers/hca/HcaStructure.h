#pragma once

#include <cstdint>
#include <memory>

#include "../IStructure.h"
#include "../../../common/utilities/runtime_jagged_array_2.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
    struct CriHcaKey;
}

namespace vgaudio::containers::hca {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::containers;
    using namespace vgaudio::codecs::crihca;

    struct HcaStructure : public IStructure {

        HcaStructure();

        ~HcaStructure() override = default;

        int32_t version;
        int32_t headerSize;
        shared_ptr<HcaInfo> hca;
        shared_ptr<CriHcaKey> encryptionKey;
        int32_t reserved1;
        int32_t reserved2;
        jarray2_ptr<uint8_t> audioData;

    };

}
