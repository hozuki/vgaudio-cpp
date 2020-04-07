#pragma once

#include <cstdint>
#include <memory>

#include "../IStructure.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
    struct CriHcaKey;
}

namespace vgaudio::containers::hca {

    struct HcaStructure : public IStructure {

        HcaStructure();

        ~HcaStructure() override = default;

        int32_t version;
        int32_t headerSize;
        std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> hca;
        std::shared_ptr<vgaudio::codecs::crihca::CriHcaKey> encryptionKey;
        int32_t reserved1;
        int32_t reserved2;
        common_lib::utilities::jarray2_ptr<uint8_t> audioData;

    };

}
