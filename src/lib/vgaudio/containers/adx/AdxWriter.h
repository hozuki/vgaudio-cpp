#pragma once

#include <memory>

#include "../AudioWriter.h"
#include "AdxConfiguration.h"

namespace vgaudio::formats::criadx {
    struct CriAdxFormat;
}

namespace vgaudio::containers::adx {

    namespace __internal {

        struct AdxWriterFormatHelper;

    }

    struct AdxWriter : public AudioWriter<AdxConfiguration> {

        DECLARE_CLASS_TEMPLATE(AdxWriter, AudioWriter, AdxConfiguration);

    private:

        std::shared_ptr<vgaudio::formats::criadx::CriAdxFormat> _adpcm;
        std::shared_ptr<__internal::AdxWriterFormatHelper> _h;

    public:

        AdxWriter() = default;

        ~AdxWriter() override = default;

    protected:

        int32_t getFileSize() override;

        void setupWriter(const std::shared_ptr<vgaudio::formats::AudioData> &audio) override;

        void writeStream(const std::shared_ptr<common_lib::io::Stream> &stream) override;

    };

}
