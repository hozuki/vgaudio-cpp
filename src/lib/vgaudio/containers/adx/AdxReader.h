#pragma once

#include <memory>

#include "../AudioReader.h"
#include "AdxStructure.h"
#include "AdxConfiguration.h"

namespace vgaudio {
    namespace codecs {
        namespace criadx {
            struct CriAdxKey;
        }
    }
}

namespace vgaudio {
    namespace containers {
        namespace adx {

            struct AdxReader : public AudioReader<AdxStructure, AdxConfiguration> {

                DECLARE_CLASS_TEMPLATE(AdxReader, AudioReader, AdxStructure, AdxConfiguration);

            private:

                std::shared_ptr<vgaudio::codecs::criadx::CriAdxKey> _encryptionKey;

            public:

                AdxReader() = default;

                ~AdxReader() override = default;

                [[nodiscard]]
                std::shared_ptr<vgaudio::codecs::criadx::CriAdxKey> getEncryptionKey() const;

                void setEncryptionKey(const std::shared_ptr<vgaudio::codecs::criadx::CriAdxKey> &encryptionKey);

            protected:

                std::shared_ptr<AdxStructure> readFile(const std::shared_ptr<common_lib::io::Stream> &stream, bool readAudioData) override;

                std::shared_ptr<vgaudio::formats::IAudioFormat> toAudioStream(const std::shared_ptr<AdxStructure> &structure) override;

                std::shared_ptr<AdxConfiguration> getConfiguration(const std::shared_ptr<AdxStructure> &structure) override;

            };

        }
    }
}
