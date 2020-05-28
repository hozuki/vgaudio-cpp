#pragma once

#include <cstdint>
#include <memory>

#include "../Configuration.h"
#include "../../codecs/crihca/CriHcaQuality.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {
            struct CriHcaKey;
        }
    }
}

namespace vgaudio {
    namespace containers {
        namespace hca {

            struct HcaConfiguration : public Configuration {

                DECLARE_CLASS(HcaConfiguration, Configuration);

                HcaConfiguration();

                HcaConfiguration(const HcaConfiguration &other) = default;

                explicit HcaConfiguration(const Configuration &other);

                ~HcaConfiguration() override = default;

                std::shared_ptr<vgaudio::codecs::crihca::CriHcaKey> encryptionKey;
                vgaudio::codecs::crihca::CriHcaQuality quality;
                int32_t bitrate;
                bool limitBitrate;

            };

        }
    }
}
