#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/runtime_array.h"
#include "../AudioReader.h"
#include "HcaStructure.h"
#include "HcaConfiguration.h"

namespace vgaudio::codecs::crihca {
    struct CriHcaKey;
}

namespace vgaudio::containers::hca {

    struct HcaReader : public AudioReader<HcaStructure, HcaConfiguration> {

        DECLARE_CLASS_TEMPLATE(HcaReader, AudioReader, HcaStructure, HcaConfiguration);

    private:

        std::shared_ptr<vgaudio::codecs::crihca::CriHcaKey> _encryptionKey;

    public:

        HcaReader() = default;

        ~HcaReader() override = default;

        [[nodiscard]]
        std::shared_ptr<vgaudio::codecs::crihca::CriHcaKey> getEncryptionKey() const;

        void setEncryptionKey(const std::shared_ptr<vgaudio::codecs::crihca::CriHcaKey> &encryptionKey);

    protected:

        std::shared_ptr<HcaStructure> readFile(const std::shared_ptr<common_lib::io::Stream> &stream, bool readAudioData) override;

        std::shared_ptr<vgaudio::formats::IAudioFormat> toAudioStream(const std::shared_ptr<HcaStructure> &structure) override;

        std::shared_ptr<HcaConfiguration> getConfiguration(const std::shared_ptr<HcaStructure> &structure) override;

    };

}
