#pragma once

#include <memory>

#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioFormatBaseBuilder.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {
            struct HcaInfo;
        }
    }
}

namespace vgaudio {
    namespace formats {
        namespace crihca {

            struct CriHcaFormat;

            struct CriHcaFormatBuilder : public AudioFormatBaseBuilder {

                DECLARE_CLASS(CriHcaFormatBuilder, AudioFormatBaseBuilder);

            private:

                common_lib::utilities::jarray2_ptr<uint8_t> _audioData;
                std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> _hca;

            public:

                CriHcaFormatBuilder(const common_lib::utilities::jarray2_ptr<uint8_t> &audioData, const std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> &hca);

                ~CriHcaFormatBuilder() override = default;

                [[nodiscard]]
                common_lib::utilities::jarray2_ptr<uint8_t> getAudioData() const;

                [[nodiscard]]
                std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> getHca() const;

                [[nodiscard]]
                int32_t getChannelCount() const override;

                std::shared_ptr<IAudioFormat> build() override;

            };

        }
    }
}
