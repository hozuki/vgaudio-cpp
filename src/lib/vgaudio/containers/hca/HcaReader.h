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

    using namespace std;
    using namespace vgaudio::containers;
    using namespace vgaudio::codecs::crihca;

    struct HcaReader : public AudioReader<HcaStructure, HcaConfiguration> {

        using A = AudioReader<HcaStructure, HcaConfiguration>;

        DECLARE_CLASS_TEMPLATE(HcaReader, AudioReader, HcaStructure, HcaConfiguration);

    private:

        shared_ptr<CriHcaKey> _encryptionKey;

    public:

        HcaReader() = default;

        ~HcaReader() override = default;

        [[nodiscard]]
        shared_ptr<CriHcaKey> getEncryptionKey() const;

        void setEncryptionKey(const shared_ptr<CriHcaKey> &encryptionKey);

    protected:

        shared_ptr<HcaStructure> readFile(const shared_ptr<Stream> &stream, bool readAudioData) override;

        shared_ptr<IAudioFormat> toAudioStream(const shared_ptr<HcaStructure> &structure) override;

        shared_ptr<HcaConfiguration> getConfiguration(const shared_ptr<HcaStructure> &structure) override;

    };

}
