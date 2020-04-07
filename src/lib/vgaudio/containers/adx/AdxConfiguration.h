#pragma once

#include <memory>

#include "../Configuration.h"
#include "../../codecs/criadx/CriAdxType.h"

namespace vgaudio::codecs::criadx {
    struct CriAdxKey;
}

namespace vgaudio::containers::adx {

    struct AdxConfiguration : public Configuration {

        DECLARE_CLASS(AdxConfiguration, Configuration);

        AdxConfiguration();

        AdxConfiguration(const AdxConfiguration &) = default;

        explicit AdxConfiguration(const Configuration &other);

        ~AdxConfiguration() override = default;

        int32_t version;
        int32_t encryptionType;
        std::shared_ptr<vgaudio::codecs::criadx::CriAdxKey> encryptionKey;
        int32_t frameSize;
        int32_t filter;
        vgaudio::codecs::criadx::CriAdxType type;

    };

}
