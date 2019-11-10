#pragma once

#include <cstdint>

#include "../Configuration.h"
#include "../../codecs/crihca/CriHcaQuality.h"

namespace vgaudio::codecs::crihca {
    struct CriHcaKey;
}

namespace vgaudio::containers::hca {

    using namespace std;
    using namespace vgaudio::containers;
    using namespace vgaudio::codecs::crihca;

    struct HcaConfiguration : public Configuration {

        DECLARE_CLASS(HcaConfiguration, Configuration);

        HcaConfiguration();

        HcaConfiguration(const HcaConfiguration &other) = default;

        explicit HcaConfiguration(const Configuration &other);

        ~HcaConfiguration() override = default;

        shared_ptr<CriHcaKey> encryptionKey;
        CriHcaQuality quality;
        int32_t bitrate;
        bool limitBitrate;

    };

}
